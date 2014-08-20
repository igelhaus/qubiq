#include <qubiq/lexeme_sequence.h>

/**
 * \class LexemeSequence
 *
 * \brief The LexemeSequence class representates a sequence of lexemes in the given text.
 *
 * The LexemeSequence class implements a sequence of lexemes in the text and
 * calculates metrics of the sequence needed for further term extraction. These
 * metrics are: mutual information, log-likelihood ratio and overall sequence
 * score which is calculated using the first two metrics.
 *
 * It is always assumed that the sequence consists of two subsequences, which
 * is crucial for calculating the metrics mentionred above.
 *
 * \sa Text
 * \sa Lexeme
 */

//! Constructs an empty LexemeSequence object.
LexemeSequence::LexemeSequence()
{
    _initialize();
}

//! Copy constructor. Constructs a LexemeSequence object from the other LexemeSequence object.
LexemeSequence::LexemeSequence(const LexemeSequence &other)
{
    _assign(other);
}

/**
 * \brief Constructs a real lexeme sequence consisting of two subsequences from the text.
 * \param[in] text   Text to extract the sequence from.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \param[in] n1     Length of the first subsequence.
 */
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

//! Destructs the LexemeSequence object.
LexemeSequence::~LexemeSequence()
{
    _destroy();
}

/**
 * \brief Assignment operator.
 * \param[in] other Another lexeme sequence to assign to the current object.
 * \returns Reference to the original object after assignment.
 */
LexemeSequence &LexemeSequence::operator =(const LexemeSequence &other)
{
    if (this != &other) {
        _destroy();
        _assign(other);
    }
    return *this;
}

/**
 * \brief Constructs a string representation of the sequence.
 * \param[in] text Text the sequence was extracted from.
 * \returns A string representing the sequence.
 */
QString LexemeSequence::image(const Text *text) const
{
    QString _image;
    int idx_first_offset = _offsets->at(0);
    for (int i = 0; i < _lexemes->length(); i++) {
        int idx_lexeme = text->offsets()->at(idx_first_offset + i);
        Lexeme *lexeme = text->lexemes()->at(idx_lexeme);
        _image.append(lexeme->forms()->at(0));
        _image.append(" ");
    }
    return _image;
}

/**
 * \brief Calculates sequence state while constructing it from the source text.
 * \param[in] text   Text to extract the sequence from.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \param[in] n1     Length of the first subsequence.
 * \returns          Sequence state.
 */
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

/**
 * \brief Constructs a sequence from the source text.
 * \param[in] text   Text to extract the sequence from.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \returns          Sequence state indicating sequence validity.
 */
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

/**
 * \brief Calculates metrics of the sequence.
 *
 * This method calculates mutual information (MI) between two subsequences composing
 * a sequence, their log-likelihood ration (LLR) and overall score.
 *
 * The sense of the metrics in this context is to measure how much the two
 * subsequences are related to each other.
 *
 * MI is calculated in a straightforward way.
 *
 * LLR is calculated as follows:
 *
 * - N tokens of the input text are split into two sets:
 *    -# \c f1:     Tokens that start the first subsequence
 *    -# \c not_f1: Tokens that do not start the first subsequence
 * - Tokens from \c f1 are split into two sets:
 *    -# Successfull tokens, \c f: Tokens that start the whole sequence
 *    -# Failure tokens: Tokens that do not start the whole sequence
 * - Tokens from \c not_f1 are split into two sets:
 *    -# Successfull tokens, \c f2_not_f1: Tokens that start the second subsequence
 *    -# Failure tokens: Tokens that do not start the second subsequence
 *
 * Our hypotesis test is:
 * - \c H0: The first and the second subsequences are linguistically related, i.e.
 * f/f1 and f2_not_f1/not_f1 are different probabilities.
 * - \c H1: The first and the second subsequences are linguistically unrelated, i.e.
 * probability of the second subsequence is the same in the entire text.
 *
 * \param[in] text   Text to extract the sequence from.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \param[in] n1     Length of the first subsequence.
 * \returns          Currently always returns \c LexemeSequence::LexemeSequenceState::STATE_OK.
 */
LexemeSequence::LexemeSequenceState LexemeSequence::calculate_metrics(const Text *text, int n, int n1)
{
    int f         = calculate_frequency(text, 0, n, true); /* frequency of the whole sequence */
    int f1        = calculate_frequency(text, 0, n1);      /* frequency of the first subsequence */
    int f2        = calculate_frequency(text, n1, n - n1); /* frequency of the second subsequence */
    int N         = text->length();
    int not_f1    = N - f1; /* number of offsets that do not start the first subsequence */
    int f2_not_f1 = f2 - f; /* frequency of the second subsequence adjacent to anything but the first subsequence */

    if (f1 == N) /* Special case (very rare): artificial texts like "x x x x" */
        not_f1 = 1;

    double p1_H0 = (double)f         / (double)f1;
    double p2_H0 = (double)f2_not_f1 / (double)not_f1;
    double  p_H1 = (double)f2        / (double)N;

    if (f == f1) /* Special case: the 1st subsequence is not present outside the sequence */
        p1_H0 -= PROBABILITY_ADJUSTMENT;

    if (f == f2) /* Special case: the 2nd subsequence is not present outside the sequence */
        p2_H0 += PROBABILITY_ADJUSTMENT;

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

/**
 * \brief Calculates frequency of a sequence in the source text.
 * \param[in] text            Text to extract the sequence from.
 * \param[in] offset          Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n               Length of the sequence.
 * \param[in] collect_offsets Set to \c true if offsets that start the sequences should be preserved internally.
 * \returns Number of occurences of the sequence in the \c text.
 */
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

/**
 * \brief Checks equality of two sequences in the source text.
 *
 * This methods checks whether arbitrary \c n tokens starting from the
 * \c text_offset in the text are equal the source sequence which is defined as
 * \c n tokens starting from the \c sequence_offset in the \c text.
 *
 * \param[in] text            Text to extract the sequence from.
 * \param[in] text_offset     Offset (expressed in tokens) of the tested sequence.
 * \param[in] sequence_offset Offset (expressed in tokens) of the initial sequence.
 * \param[in] n               Length of the sequence.
 * \returns \c true if two sequences map to the same sequence of lexemes and \c false otherwise.
 */
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

//! \internal Initializes class members.
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

//! \internal Assigns \c other members to \c this members.
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

//! \internal Frees memory occupied by class members.
void LexemeSequence::_destroy()
{
    delete _lexemes;
    delete _offsets;
    delete _key;
}
