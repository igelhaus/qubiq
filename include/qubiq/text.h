#ifndef _TEXT_H_
#define _TEXT_H_

#include <math.h>
#include <QtCore>

typedef unsigned long EntryIndex; /* index of any entry in any vector */
typedef unsigned long EntryCount; /* number/frequency of anything anywhere :) */

const EntryCount MIN_COUNT = 3;
const double MIN_MUTUAL_INFORMATION = 2.5;
const double MIN_LLR = 5.0;

class Lexeme: public QObject {
    Q_OBJECT

private:
    // FIXME: describe arbitrary lexeme features as a free-form QHash?

    QString _lexeme;
    bool    _is_boundary;

    /* Each lexeme is represented in a text as a set of its forms
     * occuring in certain text positions (counted as offsets relative to the first word): */
    QVector<QString>    *_forms;
    QVector<EntryIndex> *_offsets;

public:
    Lexeme(const QString &lexeme, bool is_boundary);
    ~Lexeme();

    inline QString  lexeme()     const { return _lexeme; }
    inline bool     isBoundary() const { return _is_boundary; }
    inline bool     isVirtual()  const { return _forms->length() == 0; }

    inline const QVector<QString>*      forms() const { return _forms; }
    inline const QVector<EntryIndex>* offsets() const { return _offsets; }

    void addForm(const QString &form, EntryIndex offset);
};

class Text: public QObject {
    Q_OBJECT

private:
    QVector<Lexeme*>           *_lexemes;    /* vector of all lexeme in the text */
    QVector<EntryIndex>        *_offsets;    /* offset of a form in the text -> offset of its lexeme in the vector */
    QHash<QString, EntryIndex> *idx_forms;   /* form   -> offset of its lexeme in the vector */
    QHash<QString, EntryIndex> *idx_lexemes; /* lexeme -> its offset in the vector */

    EntryCount num_forms;
    EntryCount num_boundaries;

    bool     is_boundary_token  (const QStringRef &token);
    bool     is_whitespace_token(const QStringRef &token);
    QString* normalize_token    (const QStringRef &token, bool is_boundary);
    bool     process_token      (const QStringRef &token);

public:
    Text();
    ~Text();

    inline EntryCount length()         const { return _offsets->length(); }
    inline EntryCount numForms()       const { return num_forms; }
    inline EntryCount numUniqueForms() const { return idx_forms->keys().length(); }
    inline EntryCount numBoundaries()  const { return num_boundaries; }
    inline EntryCount numLexemes()     const { return _lexemes->length(); }

    // FIXME: It is unsafe to pass pointer to vector of *pointers* to Lexeme
    inline const QVector<Lexeme*>* lexemes()    const { return _lexemes; }
    inline const QVector<EntryIndex>* offsets() const { return _offsets; }
    inline const QHash<QString, EntryIndex>* indexForms()   const { return idx_forms; }
    inline const QHash<QString, EntryIndex>* indexLexemes() const { return idx_lexemes; }

    bool appendFile(const QString &fname);
    bool append    (const QString &buffer);

};

class LexemeSequence: public QObject {
    Q_OBJECT

private:
    bool is_valid;

    EntryIndex _boundary;

    EntryCount _k1;
    EntryCount _n1;
    EntryCount _k2;
    EntryCount _n2;

    /* Metrics of a lexeme sequence:
     * 1) Mutual information
     * 2) Log-likelihood ratio
     * 3) Overall score
     */
    double _mi;
    double _llr;
    double _score;

    QVector<EntryIndex> *_lexemes;
    QByteArray          *_packed_lexemes; // for hashing sequences
    /* aux function for counting log-likelihood ratio: */
    inline double ll(double p, EntryCount k, EntryCount n) {
        return k * log(p) + (n - k) * log(1 - p);
    }

    void initialize();
    EntryCount frequency(const Text *text, EntryIndex offset, EntryIndex n) const;
    bool     is_sequence(const Text *text, EntryIndex text_offset, EntryIndex sequence_offset, EntryCount n) const;

public:
    LexemeSequence();
    LexemeSequence(const LexemeSequence &other);
    LexemeSequence(const Text *text, EntryIndex offset, EntryCount n, EntryIndex boundary);
    ~LexemeSequence();

    LexemeSequence &operator =(const LexemeSequence &other);

    inline bool isValid() const { return is_valid; }

    inline EntryCount length()   const { return _lexemes->length(); }
    inline EntryIndex boundary() const { return _boundary; }

    inline const QVector<EntryIndex>* lexemes() const { return _lexemes; }
    inline const QByteArray* packedLexemes()    const { return _packed_lexemes; }

    inline double mi()    const { return _mi; }
    inline double llr()   const { return _llr; }
    inline double score() const { return _score; }
};

inline bool operator ==(const LexemeSequence &s1, const LexemeSequence &s2)
{
    const QVector<EntryIndex> *l1 = s1.lexemes(), *l2 = s2.lexemes();
    if (l1->length() != l2->length())
        return false;
    for (int i = 0; i < l1->length(); i++) {
        if (l1->at(i) != l2->at(i))
            return false;
    }
    return true;
}

inline uint qHash(const LexemeSequence &key, uint seed) {
    return qHash(*(key.packedSequence()), seed);
}

// Store a set of pointers?
class LexemeSequences: public QObject {
    Q_OBJECT

private:
    const Text *_text;
    QSet<LexemeSequence> *_sequences;

public:
    LexemeSequences(const Text *text);
    ~LexemeSequences();

    inline const Text* text() const { return _text; }
    inline const QSet<LexemeSequence>* sequences() const { return _sequences; }

    bool add(EntryIndex offset, EntryCount n, EntryIndex boundary);
};

#endif // _TEXT_H_
