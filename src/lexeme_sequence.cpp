#include <qubiq/lexeme_sequence.h>

LexemeSequence::LexemeSequence()
{
    _initialize();
}

LexemeSequence::LexemeSequence(const LexemeSequence &other)
{
    _assign(other);
}

LexemeSequence::LexemeSequence(const Text *text, ulong offset, ulong n, ulong boundary)
{
    _initialize();

    _state = calculate_state(text, offset, n, boundary);

    if (_state == LexemeSequence::STATE_OK) {
        _state = build_sequence(text, offset, n);
        if (_state == LexemeSequence::STATE_OK) {
            calculate_metrics(text, n, boundary);
        }
    }
}

LexemeSequence::~LexemeSequence()
{
    _destroy();
}

LexemeSequence &LexemeSequence::operator =(const LexemeSequence &other)
{
    if (this != &other) {
        _destroy();
        _assign(other);
    }
    return *this;
}

LexemeSequence::LexemeSequenceState LexemeSequence::calculate_state(const Text *text, ulong offset, ulong n, ulong boundary)
{
    if (n < 2)
        return LexemeSequence::STATE_UNIGRAM;

    if (boundary < 1 || boundary >= n)
        return LexemeSequence::STATE_BAD_BOUNDARY;

    if (offset >= (ulong)text->offsets()->length())
        return LexemeSequence::STATE_BAD_OFFSET;

    if (offset + n > (ulong)text->offsets()->length())
        return LexemeSequence::STATE_BAD_OFFSET_N;

    return LexemeSequence::STATE_OK;
}

LexemeSequence::LexemeSequenceState LexemeSequence::build_sequence(const Text *text, ulong offset, ulong n)
{
    for (ulong i = 0; i < n; i++) {
        ulong  idx_lexeme = text->offsets()->at(offset + i);
        Lexeme *lexeme    = text->lexemes()->at(idx_lexeme);
        if (lexeme->isBoundary()) {
            /* For invalid sequences we shrink containers to 0: */
            _lexemes->resize(0);
            _seq_key->resize(0);
            return LexemeSequence::STATE_HAS_BOUNDARIES;
        }
        _lexemes->append(idx_lexeme);
        // FIXME: test packing routine
        for (int j = 0; j != sizeof(idx_lexeme); ++j) {
            _seq_key->append((char)(idx_lexeme & (0xFF << j) >> j));
        }
    }
    return LexemeSequence::STATE_OK;
}

LexemeSequence::LexemeSequenceState LexemeSequence::calculate_metrics(const Text *text, ulong n, ulong boundary)
{
    _boundary = boundary;

    _k1 = frequency(text, 0, n);        /* frequency of the whole sequence */
    _n1 = frequency(text, 0, boundary); /* frequency of the first subsequence */

    /* _k2: frequency of the second subsequence adjacent to anything but the first subsequence */
    ulong f_y = frequency(text, boundary, n - boundary);
    _k2 = f_y - _k1;

    /* _n2: number of offsets that do not start the first subsequence
     * and do not belong to the first subsequence */
    _n2 = text->offsets()->length() - boundary * _n1;

    // NB! _k1 must *NOT* be 0 here
    // NB! _k1 <= _n1 *MUST* hold here

    _mi  = text->length() * (double)_k1 / (double)_n1 / (double)f_y;
    _llr = ll((double)_k1 / (double)_n1, _k1, _n1)
        +  ll((double)_k2 / (double)_n2, _k2, _n2)
        -  ll((double)(_k1 + _k2) / (double)(_n1 + _n2), _k1, _n1)
        -  ll((double)(_k1 + _k2) / (double)(_n1 + _n2), _k2, _n2)
    ;
    _score = _mi >= MIN_MUTUAL_INFORMATION? _llr : 0.0;

    return LexemeSequence::STATE_OK;
}

ulong LexemeSequence::frequency(const Text *text, ulong offset, ulong n) const
{
    const QVector<ulong>* first = text->lexemes()->at(_lexemes->at(offset))->offsets();
    ulong f = first->length();
    for (int i = 0; i < first->length(); i++) {
        if (!is_sequence(text, first->at(i), offset, n))
            f--;
    }
    return f;
}

bool LexemeSequence::is_sequence(const Text *text, ulong text_offset, ulong sequence_offset, ulong n) const
{
    /* NB! sequence_offset and n are always correlated and won't lead to out-of-range errors */
    if (text_offset + n > (ulong)text->offsets()->length())
        return false;
    for (ulong i = 0; i < n; i++)
        if (text->offsets()->at(text_offset + i) != _lexemes->at(sequence_offset + i))
            return false;
    return true;
}

void LexemeSequence::_initialize()
{
    _state    = LexemeSequence::STATE_EMPTY;
    _boundary = 0;
    _k1       = 0;
    _n1       = 0;
    _k2       = 0;
    _n2       = 0;
    _mi       = 0.0;
    _llr      = 0.0;
    _score    = 0.0;
    _lexemes  = new QVector<ulong>;
    _seq_key  = new QByteArray;
}

void LexemeSequence::_assign(const LexemeSequence &other)
{
    _state    = other._state;
    _boundary = other._boundary;
    _k1       = other._k1;
    _n1       = other._n1;
    _k2       = other._k2;
    _n2       = other._n2;
    _mi       = other._mi;
    _llr      = other._llr;
    _score    = other._score;
    _lexemes  = new QVector<ulong>(*(other._lexemes));
    _seq_key  = new QByteArray(*(other._seq_key));
}

void LexemeSequence::_destroy()
{
    delete _lexemes;
    delete _seq_key;
}
