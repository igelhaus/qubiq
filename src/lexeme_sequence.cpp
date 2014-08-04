#include <qubiq/lexeme_sequence.h>

LexemeSequence::LexemeSequence()
{
    _initialize();
}

LexemeSequence::LexemeSequence(const LexemeSequence &other)
{
    _assign(other);
}

LexemeSequence::LexemeSequence(const Text *text, int offset, int n, int n1)
{
    _initialize();

    _state = calculate_state(text, offset, n, n1);

    if (_state == LexemeSequence::STATE_OK) {
        _state = build_sequence(text, offset, n);
        if (_state == LexemeSequence::STATE_OK) {
            calculate_metrics(text, n, n1);
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

LexemeSequence::LexemeSequenceState LexemeSequence::calculate_state(const Text *text, int offset, int n, int n1)
{
    if (n < 2)
        return LexemeSequence::STATE_UNIGRAM;

    if (n1 < 1 || n1 >= n)
        return LexemeSequence::STATE_BAD_BOUNDARY;

    if (offset < 0 || offset >= text->length())
        return LexemeSequence::STATE_BAD_OFFSET;

    if (offset + n > text->length())
        return LexemeSequence::STATE_BAD_OFFSET_N;

    return LexemeSequence::STATE_OK;
}

LexemeSequence::LexemeSequenceState LexemeSequence::build_sequence(const Text *text, int offset, int n)
{
    for (int i = 0; i < n; i++) {
        int  idx_lexeme = text->offsets()->at(offset + i);
        Lexeme *lexeme  = text->lexemes()->at(idx_lexeme);
        if (lexeme->isBoundary()) {
            /* For invalid sequences we shrink containers to 0: */
            _lexemes->resize(0);
            _key->resize(0);
            return LexemeSequence::STATE_HAS_BOUNDARIES;
        }
        _lexemes->append(idx_lexeme);
        for (int j = 0; j != sizeof(idx_lexeme); ++j) {
            _key->append((char)(idx_lexeme & (0xFF << j) >> j));
        }
    }
    return LexemeSequence::STATE_OK;
}

LexemeSequence::LexemeSequenceState LexemeSequence::calculate_metrics(const Text *text, int n, int n1)
{
    /* N tokens of the input text are split into two sets:
     *  1) f1:     Tokens that start the first subsequence
     *  2) not_f1: Tokens that do not start the first subsequence
     * Tokens from f1 are split into two sets:
     *  1) Successfull tokens, f: Tokens that start the whole sequence
     *  2) Failure tokens: Tokens that do not start the whole sequence
     * Tokens from not_f1 are split into two sets:
     *  1) Successfull tokens, f2_not_f1: Tokens that start the second subsequence
     *  2) Failure tokens: Tokens that do not start the second subsequence
     * Our test is:
     * H0: The first and the second subsequences are linguistically related, i.e.
     * f/f1 and f2_not_f1/not_f1 are different probabilities.
     * H1: The first and the second subsequences are linguistically unrelated, i.e.
     * probability of the second subsequence is the same in the entire text.
    */
    int f         = calculate_frequency(text, 0, n, true); /* frequency of the whole sequence */
    int f1        = calculate_frequency(text, 0, n1);      /* frequency of the first subsequence */
    int f2        = calculate_frequency(text, n1, n - n1); /* frequency of the second subsequence */
    int N         = text->length();
    int not_f1    = N - f1; /* number of offsets that do not start the first subsequence */
    int f2_not_f1 = f2 - f; /* frequency of the second subsequence adjacent to anything but the first subsequence */
    double p1_H0  = (double)f         / (double)f1;
    double p2_H0  = (double)f2_not_f1 / (double)not_f1;
    double  p_H1  = (double)f2        / (double)N;

    _n1  = n1;
    _mi  = (double)N * (double)f / (double)f1 / (double)f2;
    _llr = ll(p1_H0, f, f1)
        +  ll(p2_H0, f2_not_f1, not_f1)
        -  ll( p_H1, f, f1)
        -  ll( p_H1, f2_not_f1, not_f1)
    ;
    _score = _mi >= MIN_MUTUAL_INFORMATION? _llr : 0.0;

    return LexemeSequence::STATE_OK;
}

int LexemeSequence::calculate_frequency(const Text *text, int offset, int n, bool collect_offsets)
{
    const QVector<int>* first = text->lexemes()->at(_lexemes->at(offset))->offsets();
    int f = first->length();
    for (int i = 0; i < first->length(); i++) {
        if (!is_sequence(text, first->at(i), offset, n)) {
            f--;
            continue;
        }
        if (collect_offsets)
            _offsets->append(first->at(i));
    }
    return f;
}

bool LexemeSequence::is_sequence(const Text *text, int text_offset, int sequence_offset, int n) const
{
    /* NB! sequence_offset and n are always correlated and won't lead to out-of-range errors */
    if (text_offset + n > text->length())
        return false;
    for (int i = 0; i < n; i++)
        if (text->offsets()->at(text_offset + i) != _lexemes->at(sequence_offset + i))
            return false;
    return true;
}

void LexemeSequence::_initialize()
{
    _state    = LexemeSequence::STATE_EMPTY;
    _n1       = 0;
    _mi       = 0.0;
    _llr      = 0.0;
    _score    = 0.0;
    _led      = 0;
    _red      = 0;
    _lexemes  = new QVector<int>;
    _offsets  = new QVector<int>;
    _key      = new QByteArray;
}

void LexemeSequence::_assign(const LexemeSequence &other)
{
    _state    = other._state;
    _n1       = other._n1;
    _mi       = other._mi;
    _llr      = other._llr;
    _score    = other._score;
    _led      = other._led;
    _red      = other._red;
    _lexemes  = new QVector<int>(*(other._lexemes));
    _offsets  = new QVector<int>(*(other._offsets));
    _key      = new QByteArray(*(other._key));
}

void LexemeSequence::_destroy()
{
    delete _lexemes;
    delete _offsets;
    delete _key;
}
