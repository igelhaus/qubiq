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
 * Although we referer to sequences as "sequences in the text" this class does not depend
 * on \c Text directly. Instead, its main dependency is \c LexemeIndex class which is assumed
 * to cover the originall text fully.
 *
 * \sa LexemeIndex
 * \sa Lexeme
 * \sa Text
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
 * \param[in] index  Lexeme index to derive the sequence from.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \param[in] n1     Length of the first subsequence.
 */
LexemeSequence::LexemeSequence(const LexemeIndex *index, int offset, int n, int n1)
{
    _initialize();

    _state = calculate_state(index, offset, n, n1);

    if (_state == LexemeSequence::STATE_OK) {
        _state = build_sequence(offset, n);
        if (_state == LexemeSequence::STATE_OK) {
            calculate_metrics(offset, n, n1);
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
 * \returns A string representing the sequence.
 */
QString LexemeSequence::image() const
{
    QString _image;
    if (_state != LexemeSequence::STATE_OK)
        return _image;

    int first_pos      = _index->positions(_seq->at(0)->name())->at(0);
    for (int i = 0; i < _seq->length(); i++) {
        _image.append(_index->findByPosition(first_pos + i)->name()).append(" ");
    }

    return _image;
}

/**
 * \brief Calculates sequence state while constructing it from the source text.
 * \param[in] index  Lexeme index to derive the sequence from.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \param[in] n1     Length of the first subsequence.
 * \returns          Sequence state.
 */
LexemeSequence::LexemeSequenceState LexemeSequence::calculate_state(const LexemeIndex *index, int offset, int n, int n1)
{
    if (index == NULL)
        return LexemeSequence::STATE_BAD_INDEX;

    // NB! To make this class depend only LexemeIndex we assume that
    // the index is *not* sparse, i.e. covers all token positions in the original text.
    _index   = index;
    _txt_len = _index->numUniquePositions();

    if (n < 2)
        return LexemeSequence::STATE_UNIGRAM;

    if (n1 < 1 || n1 >= n)
        return LexemeSequence::STATE_BAD_BOUNDARY;

    if (offset < 0 || offset >= _txt_len)
        return LexemeSequence::STATE_BAD_OFFSET;

    if (offset + n > _txt_len)
        return LexemeSequence::STATE_BAD_OFFSET_N;

    return LexemeSequence::STATE_OK;
}

/**
 * \brief Constructs a sequence from the source text.
 * \param[in] offset Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n      Length of the sequence.
 * \returns          Sequence state indicating sequence validity.
 */
LexemeSequence::LexemeSequenceState LexemeSequence::build_sequence(int offset, int n)
{
    for (int i = 0; i < n; i++) {
        Lexeme *lexeme = _index->findByPosition(offset + i);
        if (lexeme->isBoundary()) {
            /* For invalid sequences we shrink containers to 0: */
            _seq->resize(0);
            _key->resize(0);
            return LexemeSequence::STATE_HAS_BOUNDARIES;
        }
        _seq->append(lexeme);
        add_to_key(lexeme);
    }
    return LexemeSequence::STATE_OK;
}

/**
 * \brief Appends data about lexeme to the key of the sequence.
 *
 * Lexeme's address (which size is equal to the size of \c qintptr) is converted to bytes
 * and appended to the internal \c _key byte storage. All sequences that consist of the same
 * lexemes in the same order will share the same \c _key since lexeme entries are unique and not
 * relocatable in the \c _index.
 *
 * \param[in] lexeme Lexeme to proces.
 */
void LexemeSequence::add_to_key(Lexeme *lexeme)
{
    qintptr lexeme_key = (qintptr)lexeme;
    for (int i = 0; i != sizeof(lexeme_key); i++) {
        qintptr shift = i << 3;
        qintptr mask  = ((qintptr)0xFF) << shift;
        char    byte  = (char)((lexeme_key & mask) >> shift);
        _key->append(byte);
    }
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
 * \param[in] offset  Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n       Length of the sequence.
 * \param[in] n1      Length of the first subsequence.
 * \returns           Currently always returns \c LexemeSequence::LexemeSequenceState::STATE_OK.
 */
LexemeSequence::LexemeSequenceState LexemeSequence::calculate_metrics(int offset, int n, int n1)
{
    int f         = calculate_frequency(offset, n, true);     /* frequency of the whole sequence     */
    int f1        = calculate_frequency(offset, n1);          /* frequency of the first subsequence  */
    int f2        = calculate_frequency(offset + n1, n - n1); /* frequency of the second subsequence */
    int N         = _txt_len;
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

    _f   = f;
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
 * \param[in] offset      Offset (expressed in tokens) to start building the sequence at.
 * \param[in] n           Length of the sequence.
 * \param[in] collect_pos If \c true internal storage of sequence position in the text will be updated.
 * \returns Number of occurences of the sequence in the \c text.
 */
int LexemeSequence::calculate_frequency(int offset, int n, bool collect_pos /* = false*/)
{
    Lexeme       *lexeme    = _index->findByPosition(offset);
    QVector<int> *first_pos = _index->positions(lexeme->name());
    int f = first_pos->size();
    for (int i = 0; i < first_pos->length(); i++) {
        int pos = first_pos->at(i);
        if (!is_sequence(pos, offset, n)) {
            f--;
            continue;
        }
        if (collect_pos) {
            _pos->append(pos);
        }
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
 * \param[in] text_offset     Offset (expressed in tokens) of the tested sequence.
 * \param[in] sequence_offset Offset (expressed in tokens) of the initial sequence.
 * \param[in] n               Length of the sequence.
 * \returns \c true if two sequences map to the same sequence of lexemes and \c false otherwise.
 */
bool LexemeSequence::is_sequence(int text_offset, int sequence_offset, int n) const
{
    /* NB! sequence_offset and n are always correlated and won't lead to out-of-range errors */
    if (text_offset + n > _txt_len)
        return false;
    for (int i = 0; i < n; i++) {
        if (_index->findByPosition(text_offset + i) != _index->findByPosition(sequence_offset + i)) {
            return false;
        }
    }
    return true;
}

//! \internal Initializes class members.
void LexemeSequence::_initialize()
{
    _index    = NULL;
    _state    = LexemeSequence::STATE_EMPTY;
    _n1       = 0;
    _f        = 0;
    _mi       = 0.0;
    _llr      = 0.0;
    _score    = 0.0;
    _led      = 0;
    _red      = 0;
    _txt_len  = 0;
    _seq      = new QVector<Lexeme*>();
    _pos      = new QVector<int>();
    _key      = new QByteArray;
}

//! \internal Assigns \c other members to \c this members.
void LexemeSequence::_assign(const LexemeSequence &other)
{
    _index    = other._index;
    _state    = other._state;
    _n1       = other._n1;
    _f        = other._f;
    _mi       = other._mi;
    _llr      = other._llr;
    _score    = other._score;
    _led      = other._led;
    _red      = other._red;
    _txt_len  = other._txt_len;
    _seq      = new QVector<Lexeme*>(*(other._seq));
    _pos      = new QVector<int>(*(other._pos));
    _key      = new QByteArray(*(other._key));
}

//! \internal Frees memory occupied by class members.
void LexemeSequence::_destroy()
{
    delete _seq;
    delete _pos;
    delete _key;
}
