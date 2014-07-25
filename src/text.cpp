#include <qubiq/text.h>

const qint64 DEFAULT_READ_BUFFER_SIZE = 80;

Text::Text() {
    _lexemes    = new QVector<Lexeme*>();
    _offsets    = new QVector<ulong>();
    idx_forms   = new QHash<QString, ulong>();
    idx_lexemes = new QHash<QString, ulong>();

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

    ulong idx_lexeme;
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
