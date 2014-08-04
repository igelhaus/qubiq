#ifndef _LEXEME_SEQUENCE_H_
#define _LEXEME_SEQUENCE_H_

#include <math.h>
#include <QtCore>
#include <qubiq/text.h>

const int    MIN_COUNT              =   3;
const double MIN_MUTUAL_INFORMATION = 2.5;
const double MIN_LLR                = 5.0;

class LexemeSequence: public QObject {
    Q_OBJECT

public:
    enum LexemeSequenceState {
        STATE_OK             = 0,
        STATE_EMPTY          = 1,
        STATE_UNIGRAM        = 2,
        STATE_BAD_BOUNDARY   = 3,
        STATE_BAD_OFFSET     = 4,
        STATE_BAD_OFFSET_N   = 5,
        STATE_HAS_BOUNDARIES = 6
    };

    LexemeSequence();
    LexemeSequence(const LexemeSequence &other);
    LexemeSequence(const Text *text, int offset, int n, int n1);
    ~LexemeSequence();

    LexemeSequence &operator =(const LexemeSequence &other);

    inline LexemeSequenceState state() const { return _state; }

    inline bool isValid()  const { return _state == LexemeSequence::STATE_OK; }
    inline int  length()   const { return _lexemes->length(); }
    inline int  n1()       const { return _n1; }
    inline int frequency() const { return _offsets->length(); }

    inline const QVector<int>* lexemes() const { return _lexemes; }
    inline const QVector<int>* offsets() const { return _offsets; }
    inline const QByteArray*   key()     const { return _key; }

    inline double mi()    const { return _mi; }
    inline double llr()   const { return _llr; }
    inline double score() const { return _score; }

    inline int leftExpansionDistance()  const { return _led; }
    inline int rightExpansionDistance() const { return _red; }

    inline void  incLeftExpansionDistance(int n = 1) { _led += n; }
    inline void incRightExpansionDistance(int n = 1) { _red += n; }

private:
    LexemeSequenceState _state;

    int _n1;

    /* Metrics of a lexeme sequence:
     * 1) Mutual information
     * 2) Log-likelihood ratio
     * 3) Overall score
     */
    double _mi;
    double _llr;
    double _score;

    // Expansion history of the sequence
    int _led; /* Left Expansion Distance */
    int _red; /* Right Expansion Disatnce */

    QVector<int> *_lexemes;
    QVector<int> *_offsets;
    QByteArray   *_key; /* Sequence key for hashing */

    /* aux function for counting log-likelihood ratio: */
    inline double ll(double p, int k, int n) const {
        return k * log(p) + (n - k) * log(1 - p);
    }

    void _initialize();
    void _destroy();
    void _assign(const LexemeSequence &other);

    LexemeSequenceState calculate_state  (const Text *text, int offset, int n, int n1);
    LexemeSequenceState build_sequence   (const Text *text, int offset, int n);
    LexemeSequenceState calculate_metrics(const Text *text, int n, int n1);

    int  calculate_frequency(const Text *text, int offset, int n, bool collect_offsets = false);
    bool is_sequence        (const Text *text, int text_offset, int sequence_offset, int n) const;
};

inline bool operator ==(const LexemeSequence &s1, const LexemeSequence &s2)
{
    const QVector<int> *l1 = s1.lexemes();
    const QVector<int> *l2 = s2.lexemes();
    if (l1->length() != l2->length())
        return false;
    for (int i = 0; i < l1->length(); i++) {
        if (l1->at(i) != l2->at(i))
            return false;
    }
    return true;
}

inline uint qHash(const LexemeSequence &sequence, uint seed) {
    return qHash(*(sequence.key()), seed);
}

#endif // _LEXEME_SEQUENCE_H_
