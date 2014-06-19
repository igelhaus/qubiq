#include <qubiq/text.h>

const qint64 DEFAULT_READ_BUFFER_SIZE = 80;

//
// Lexeme class
//

Lexeme::Lexeme(const QString &lexeme, bool is_boundary)
{
    _lexeme      = lexeme;
    _is_boundary = is_boundary;
    _forms       = new QVector<QString>();
    _offsets     = new QVector<EntryIndex>();
}

Lexeme::~Lexeme()
{
    delete _forms;
    delete _offsets;
}

void Lexeme::addForm(const QString &form, EntryIndex offset)
{
    _forms->append(form);
    _offsets->append(offset);
}

//
// Text class
//

Text::Text() {
    _lexemes    = new QVector<Lexeme*>();
    _offsets    = new QVector<EntryIndex>();
    idx_forms   = new QHash<QString, EntryIndex>();
    idx_lexemes = new QHash<QString, EntryIndex>();

    num_forms      = 0;
    num_boundaries = 0;
}

Text::~Text() {
    for (int i = 0; i < _lexemes->length(); i++)
        delete _lexemes->at(i);

    delete _lexemes;
    delete _offsets;
    delete idx_forms;
    delete idx_lexemes;
}

bool Text::appendFile(const QString &fname)
{
    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Error: unable to append from file
        return false;
    }

    QTextStream file_stream(&file);
    QString     buffer = file_stream.readLine(DEFAULT_READ_BUFFER_SIZE);
    QString     token_part;
    while (buffer.length() > 0) {
        QTextBoundaryFinder *boundary_finder = new QTextBoundaryFinder(
            QTextBoundaryFinder::Word, buffer
        );
        int pos_start = boundary_finder->position();
        int pos_end   = boundary_finder->toNextBoundary();
        while (pos_end != -1) {
            QStringRef token = buffer.midRef(pos_start, pos_end - pos_start);
            pos_start = pos_end;
            pos_end   = boundary_finder->toNextBoundary();
            if (pos_end != -1) {
                qDebug() << "token = " << token;
                process_token(token);
            } else {
                token_part = token.toString();
            }
        };
        delete boundary_finder;

        buffer.clear();
        QString _buffer = file_stream.readLine(DEFAULT_READ_BUFFER_SIZE);
        if (!_buffer.isNull()) {
            buffer.append(token_part).append(_buffer);
        } else {
            /* Last chunk is guaranteed to be a valid token, process it: */
            qDebug() << "very_last_token = " << token_part;
            QStringRef token = token_part.midRef(0, token_part.length());
            process_token(token);
        }
    }

    return true;
}

bool Text::append(const QString &buffer)
{
    QTextBoundaryFinder *boundary_finder = new QTextBoundaryFinder(
        QTextBoundaryFinder::Word, buffer
    );
    int pos_start = boundary_finder->position();
    int pos_end   = boundary_finder->toNextBoundary();
    while (pos_end != -1) {
        QStringRef token = buffer.midRef(pos_start, pos_end - pos_start);
        pos_start = pos_end;
        pos_end   = boundary_finder->toNextBoundary();
        qDebug() << "token = " << token;
        process_token(token);
    };
    delete boundary_finder;
    return true;
}

bool Text::is_whitespace_token(const QStringRef &token)
{
    const QChar *chars = token.unicode();
    for (int i = 0; i < token.length(); i++) {
        if (!chars[i].isSpace())
            return false;
    }
    return true;
}

bool Text::is_boundary_token(const QStringRef &token)
{
    const QChar *chars = token.unicode();
    for (int i = 0; i < token.length(); i++) {
        if (!chars[i].isPunct())
            return false;
    }
    return true;
}

QString* Text::normalize_token(const QStringRef &token, bool is_boundary)
{
    QString *normalized;

    if (is_boundary) {
        normalized = new QString(token.toString());
    } else {
        normalized = new QString(token.toString());
        /* 2DO:
         * * add capitalization normalization
         * * add morphology routines
        */
    }

    return normalized;
}

/* Process a token:
 * 1. Ignore a whitespace token
 * 2. If a token is normalized to a new lexeme, insert it into the index of lexemes
 * 3. Add information to the index of forms
 * 4. For the lexeme index entry, add
 */
bool Text::process_token(const QStringRef &token)
{
    if (is_whitespace_token(token))
        return false;

    bool is_boundary = is_boundary_token(token);

    if (is_boundary)
        num_boundaries++;
    else
        num_forms++;

    EntryIndex idx_lexeme;
    QString form = token.toString();
    if (idx_forms->contains(form)) {
        idx_lexeme = idx_forms->value(form);
    } else {
        QString *normalized = normalize_token(token, is_boundary);

        if (!idx_lexemes->contains(*normalized)) {
            Lexeme *lexeme = new Lexeme(*normalized, is_boundary);
            _lexemes->append(lexeme);
            idx_lexemes->insert(*normalized, lexemes->length() - 1);
        }

        idx_lexeme = idx_lexemes->value(*normalized);
        idx_forms->insert(form, idx_lexeme);

        delete normalized;
    }

    _offsets->append(idx_lexeme);
    _lexemes->at(idx_lexeme)->addForm(form, offsets->length() - 1);

    return true;
}

//
// LexemeSequence class
//

LexemeSequence::LexemeSequence()
{
    initialize();
}

LexemeSequence::LexemeSequence(const LexemeSequence &other)
{
    is_valid  = other.is_valid;
    _boundary = other._boundary;
    _k1       = other._k1;
    _n1       = other._n1;
    _k2       = other._k2;
    _n2       = other._n2;
    _mi       = other._mi;
    _llr      = other._llr;
    _score    = other._score;

    _lexemes = new QVector<EntryIndex>;
    for (int i = 0; i < other._lexemes->length(); i++)
        _lexemes->append(other._lexemes->at(i));

    _packed_lexemes = new QByteArray;
    for (int i = 0; i < other._packed_lexemes->length(); i++)
        _packed_lexemes->append(other._packed_lexemes->at(i));

}

LexemeSequence::LexemeSequence(const Text *text, EntryIndex offset, EntryCount n, EntryIndex boundary)
{
    initialize();

    // FIXME: implement error tracking
    if (offset >= text->offsets()->length())
        ; // Error: offset out of range
    if (offset + n > text->offsets()->length())
        ; // Error: offset + n out of range
    if (n < 2)
        ; // Error: Unigrams are not sequences
    if (boundary < 1 || boundary >= n)
        ; // Error: incorrect boundary

    for (int i = 0; i < n; i++) {
        EntryIndex idx_lexeme = text->offsets()->at(offset + i);
        Lexeme     *lexeme    = text->lexemes()->at(idx_lexeme);
        if (lexeme->isBoundary()) {
            break; // Error: valid sequence does not include boundaries
        }
        // FIXME: test packing routine
        _lexemes->append(idx_lexeme);
        for (int j = 0; j != sizeof(idx_lexeme); ++j) {
            _packed_lexemes->append((char)(idx_lexeme & (0xFF << j) >> j));
        }
    }
    if (_lexemes->length() != n) {
        /* For invalid sequences we shrink containers to 0: */
        _lexemes->resize(0);
        _packed_lexemes->resize(0);
    } else {
        is_valid  = true;
        _boundary = boundary;

        _k1 = frequency(text, 0, n);        /* frequency of the whole sequence */
        _n1 = frequency(text, 0, boundary); /* frequency of the first subsequence */

        /* _k2: frequency of the second subsequence adjacent to anything but the first subsequence */
        EntryCount f_y = frequency(text, boundary, n - boundary);
        _k2 = f_y - _k1;

        /* _n2: number of offsets that do not start the first subsequence
         * and do not belong to the first subsequence */
        _n2 = text->offsets()->length() - boundary * _n1;

        // NB! _k1 must *NOT* be 0 here
        // NB! _k1 <= _n1 *MUST* hold here

        _mi  = (double)_k1 / (double)_n1 / (double)f_y;
        _llr = ll((double)_k1 / (double)_n1, _k1, _n1)
            +  ll((double)_k2 / (double)_n2, _k2, _n2)
            -  ll((double)(_k1 + _k2) / (double)(_n1 + _n2), _k1, _n1)
            -  ll((double)(_k1 + _k2) / (double)(_n1 + _n2), _k2, _n2)
        ;
        _score = _mi >= MIN_MUTUAL_INFORMATION? _llr : 0.0;
    }
}

void LexemeSequence::initialize()
{
    is_valid  = false;
    _boundary = 0;
    _k1       = 0;
    _n1       = 0;
    _k2       = 0;
    _n2       = 0;
    _mi       = 0.0;
    _llr      = 0.0;
    _score    = 0.0;
    _lexemes        = new QVector<EntryIndex>;
    _packed_lexemes = new QByteArray;
}

EntryCount LexemeSequence::frequency(const Text *text, EntryIndex offset, EntryIndex n) const
{
    const QVector<EntryIndex>* first = text->lexemes()->at(_lexemes->at(offset))->offsets();
    EntryCount f = first->length();
    for (int i = 0; i < first->length(); i++) {
        if (!is_sequence(text, first->at(i), offset, n))
            f--;
    }
    return f;
}

bool LexemeSequence::is_sequence(const Text *text, EntryIndex text_offset, EntryIndex sequence_offset, EntryCount n) const
{
    /* NB! sequence_offset and n are always correlated and won't lead to out-of-range errors */
    if (text_offset + n > text->offsets()->length())
        return false;
    for (int i = 0; i < n; i++)
        if (text->offsets()->at(text_offset + i) != _lexemes->at(sequence_offset + i))
            return false;
    return true;
}

LexemeSequence::~LexemeSequence()
{
    delete _lexemes;
    delete _packed_lexemes;
}

LexemeSequence &LexemeSequence::operator =(const LexemeSequence &other)
{
    // FIXME:
}
