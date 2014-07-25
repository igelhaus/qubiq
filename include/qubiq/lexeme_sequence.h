#ifndef _LEXEME_SEQUENCE_H_
#define _LEXEME_SEQUENCE_H_

#include <math.h>
#include <QtCore>
#include <qubiq/text.h>

const ulong  MIN_COUNT              =   3;
const double MIN_MUTUAL_INFORMATION = 2.5;
const double MIN_LLR                = 5.0;

enum LexemeSequenceState {
    OK             = 0,
    EMPTY          = 1,
    UNIGRAM        = 2,
    BAD_BOUNDARY   = 3,
    BAD_OFFSET     = 4,
    BAD_OFFSET_N   = 5,
    HAS_BOUNDARIES = 6
};

class LexemeSequence: public QObject {
    Q_OBJECT

private:
    LexemeSequenceState _state;

    ulong _boundary;

    ulong _k1;
    ulong _n1;
    ulong _k2;
    ulong _n2;

    /* Metrics of a lexeme sequence:
     * 1) Mutual information
     * 2) Log-likelihood ratio
     * 3) Overall score
     */
    double _mi;
    double _llr;
    double _score;

    QVector<ulong> *_lexemes;
    QByteArray     *_seq_key; // Sequence key for hashing

    /* aux function for counting log-likelihood ratio: */
    inline double ll(double p, ulong k, ulong n) {
        return k * log(p) + (n - k) * log(1 - p);
    }

    void _initialize();
    void _destroy();
    void _assign(const LexemeSequence &other);

    LexemeSequenceState calculate_state  (const Text *text, ulong offset, ulong n, ulong boundary);
    LexemeSequenceState build_sequence   (const Text *text, ulong offset, ulong n);
    LexemeSequenceState calculate_metrics(const Text *text, ulong n, ulong boundary);

    ulong frequency  (const Text *text, ulong offset, ulong n) const;
    bool  is_sequence(const Text *text, ulong text_offset, ulong sequence_offset, ulong n) const;

public:
    LexemeSequence();
    LexemeSequence(const LexemeSequence &other);
    LexemeSequence(const Text *text, ulong offset, ulong n, ulong boundary);
    ~LexemeSequence();

    LexemeSequence &operator =(const LexemeSequence &other);

    inline bool  isValid()  const { return _state == LexemeSequenceState::OK; }
    inline ulong length()   const { return _lexemes->length(); }
    inline ulong boundary() const { return _boundary; }

    inline const QVector<ulong>* lexemes()     const { return _lexemes; }
    inline const QByteArray*     sequenceKey() const { return _seq_key; }

    inline double mi()    const { return _mi; }
    inline double llr()   const { return _llr; }
    inline double score() const { return _score; }
};

inline bool operator ==(const LexemeSequence &s1, const LexemeSequence &s2)
{
    const QVector<ulong> *l1 = s1.lexemes();
    const QVector<ulong> *l2 = s2.lexemes();
    if (l1->length() != l2->length())
        return false;
    for (int i = 0; i < l1->length(); i++) {
        if (l1->at(i) != l2->at(i))
            return false;
    }
    return true;
}

inline uint qHash(const LexemeSequence &key, uint seed) {
    return qHash(*(key.sequenceKey()), seed);
}

#endif // _LEXEME_SEQUENCE_H_
