#ifndef _LEXEME_SEQUENCE_H_
#define _LEXEME_SEQUENCE_H_

#include <math.h>
#include <QtCore>
#include <qubiq/qubiq_global.h>
#include <qubiq/text.h>

const double PROBABILITY_ADJUSTMENT = 0.001; //!< Adjustment for correcting extreme probability values (0 and 1)
const double MIN_MUTUAL_INFORMATION = 2.5;

class QUBIQSHARED_EXPORT LexemeSequence: public QObject {
    Q_OBJECT

public:
    //! LexemeSequenceState: enumeration for indicating states of the sequence.
    enum LexemeSequenceState {
        STATE_OK             = 0, //!< Valid sequence.
        STATE_BAD_TEXT       = 1, //!< Invalid: Pointer to text is NULL.
        STATE_EMPTY          = 2, //!< Invalid: Sequence is empty.
        STATE_UNIGRAM        = 3, //!< Invalid: Contains less than 2 lexemes.
        STATE_BAD_BOUNDARY   = 4, //!< Invalid: Incorrectly split into subsequences.
        STATE_BAD_OFFSET     = 5, //!< Invalid: Sequence offset does not fit the text length.
        STATE_BAD_OFFSET_N   = 6, //!< Invalid: Offset + length do not fit the text length.
        STATE_HAS_BOUNDARIES = 7  //!< Invalid: Sequence includes boundary lexemes.
    };

    LexemeSequence();
    LexemeSequence(const LexemeSequence &other);
    LexemeSequence(const Text *text, int offset, int n, int n1);
    ~LexemeSequence();

    LexemeSequence &operator =(const LexemeSequence &other);
    QString image() const;

    //! Returns pointer to the text the sequence was extracted from.
    const Text* text() const { return _text; }

    //! Returns state of the sequence.
    //! \sa LexemeSequenceState
    //! \sa isValid
    inline LexemeSequenceState state() const { return _state; }

    //! Returns \c true if the object represents a valid lexeme sequence, and \c false otherwise.
    //! \sa LexemeSequenceState
    //! \sa state
    inline bool isValid() const { return _state == LexemeSequence::STATE_OK; }

    //! Returns the number of lexemes that compose the sequence.
    //! \sa n1
    inline int length() const { return _seq->size(); }

    //! Returns the number of lexemes that compose the first subsequence.
    //! \sa length
    inline int n1() const { return _n1; }

    //! Returns the number of ocurrences of the sequence in the source text.
    inline int frequency() const { return _f; }

    //! Returns a pointer to the vector mapping lexemes of the sequence to
    //! their offsets in the source text's vector of all lexemes.
    inline const QVector<Lexeme*>* lexemes() const { return _seq; }

    //! Returns a pointer to the vector of all offsets of the first lexeme
    //! in the source text.
    //! \sa LexemeIndex::positions
    inline const QVector<int>* positions() const { return _pos; }

    //! Returns a key of the sequence, a special internal value for implementing hashes and sets of sequences.
    inline const QByteArray* key() const { return _key; }

    //! Returns mutual information between the two subsequences composing the sequence.
    //! \sa llr
    //! \sa score
    inline double mi() const { return _mi; }

    //! Returns log-likelihood ratio between the two subsequences composing the sequence.
    //! \sa mi
    //! \sa score
    inline double llr() const { return _llr; }

    //! Returns overall score of the sequence.
    //! \sa mi
    //! \sa llr
    inline double score() const { return _score; }

    //! Returns current left expansion distance of the sequence.
    //! \sa rightExpansionDistance
    //! \sa incLeftExpansionDistance
    //! \sa incRightExpansionDistance
    inline int leftExpansionDistance() const { return _led; }

    //! Returns current right expansion distance of the sequence.
    //! \sa leftExpansionDistance
    //! \sa incLeftExpansionDistance
    //! \sa incRightExpansionDistance
    inline int rightExpansionDistance() const { return _red; }

    //! Increments current left expansion distance of the sequence by the value of \c n (1 by default).
    //! \sa leftExpansionDistance
    //! \sa rightExpansionDistance
    //! \sa incRightExpansionDistance
    inline void  incLeftExpansionDistance(int n = 1) { _led += n; }

    //! Increments current right expansion distance of the sequence by the value of \c n (1 by default).
    //! \sa leftExpansionDistance
    //! \sa rightExpansionDistance
    //! \sa incLeftExpansionDistance
    inline void incRightExpansionDistance(int n = 1) { _red += n; }

private:
    LexemeSequenceState _state;

    int _n1; //!< Length of the first subsequence

    int    _f;     //!< Frequency of the sequence
    double _mi;    //!< Mutual information
    double _llr;   //!< Log-likelihood ratio
    double _score; //!< Overall score

    // Expansion history of the sequence
    int _led; //!< Left Expansion Distance
    int _red; //!< Right Expansion Disatnce

    const Text   *_text;    //!< Original text the sequence is extracted from.
    LexemeIndex  *_index;   //!< Index built on the text to derive sequences from.
    QByteArray   *_key;     //!< Sequence key for hashing.
    QVector<Lexeme*> *_seq; //!< Vector of pointers to lexemes the sequence actually consists of.
    QVector<int>     *_pos; //!< Vector of positions of the sequence in the text.

    /**
     * \brief Auxiliary function for counting log-likelihood ratio.
     *
     * This is just a logged probability mass function for binomial distribution
     * without binomial coefficient in the formula.
     *
     * \param[in] p Assumed probability of the success.
     * \param[in] k Number of successes.
     * \param[in] n Number of trials.
     */
    inline double ll(double p, int k, int n) const {
        return k * log(p) + (n - k) * log(1 - p);
    }

    void _initialize();
    void _destroy();
    void _assign(const LexemeSequence &other);

    void add_to_key(Lexeme *lexeme);

    LexemeSequenceState calculate_state  (const Text *text, int offset, int n, int n1);
    LexemeSequenceState build_sequence   (int offset, int n);
    LexemeSequenceState calculate_metrics(int offset, int n, int n1);

    int  calculate_frequency(int offset, int n, bool collect_pos = false);
    bool is_sequence        (int text_offset, int sequence_offset, int n) const;
};

/**
 * \brief Comparison operator for the two sequences.
 * \param[in] s1 First sequence.
 * \param[in] s2 Second sequence.
 * \returns \c true if both sequences contain the same lexemes in the same order, \c false otherwise.
 */
inline bool operator ==(const LexemeSequence &s1, const LexemeSequence &s2)
{
    if (s1.length() != s1.length())
        return false;
    const QVector<Lexeme*> *l1 = s1.lexemes();
    const QVector<Lexeme*> *l2 = s2.lexemes();
    for (int i = 0; i < l1->length(); i++) {
        if (l1->at(i) != l2->at(i))
            return false;
    }
    return true;
}

/**
 * \brief Compares quality of two sequences.
 * \param[in] s1 First sequence.
 * \param[in] s2 Second sequence.
 * \returns \c true if the first sequence has higher score, \c false otherwise.
 */
inline bool hasBetterSequence(const LexemeSequence &s1, const LexemeSequence &s2)
{
    return s1.score() > s2.score();
}

/**
 * \brief Overloaded qHash function for calculating hash value of the sequence
 * \param sequence Lexeme sequence.
 * \param seed     Seed to initialize hashing procedure.
 * \returns Hash value of the sequence.
 */
inline uint qHash(const LexemeSequence &sequence, uint seed) {
    return qHash(*(sequence.key()), seed);
}

#endif // _LEXEME_SEQUENCE_H_
