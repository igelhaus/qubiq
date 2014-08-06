#ifndef _TEXT_H_
#define _TEXT_H_

#include <QtCore>
#include <qubiq/lexeme.h>

const qint64 DEFAULT_READ_BUFFER_SIZE = 80;

// FIXME: enhance debugging
// FIXME: pass text properties: language (eventually locale setting)
// FIXME: implement lemmatizing interface
// FIXME: tokenization with lemmatization vs. tokenization before lemmatization
// FIXME: add dumping function

class Text: public QObject {
    Q_OBJECT

private:
    QVector<Lexeme*>    *_lexemes;    /* vector of all lexemes in the text */
    QVector<int>        *_offsets;    /* offset of a form in the text -> offset of its lexeme in the vector */
    QHash<QString, int> *idx_forms;   /* form   -> offset of its lexeme in the vector */
    QHash<QString, int> *idx_lexemes; /* lexeme -> its offset in the vector */

    int num_forms;
    int num_boundaries;

    bool     is_boundary_token  (const QStringRef &token);
    bool     is_whitespace_token(const QStringRef &token);
    QString* normalize_token    (const QStringRef &token, bool is_boundary);
    bool     process_token      (const QStringRef &token);

public:
    Text();
    ~Text();

    inline int length()         const { return _offsets->length(); }
    inline int numForms()       const { return num_forms; }
    inline int numUniqueForms() const { return idx_forms->keys().length(); }
    inline int numBoundaries()  const { return num_boundaries; }
    inline int numLexemes()     const { return _lexemes->length(); }

    // FIXME: It is unsafe to pass pointer to vector of *pointers* to Lexeme
    inline const QVector<Lexeme*>* lexemes()         const { return _lexemes; }
    inline const QVector<int>* offsets()             const { return _offsets; }
    inline const QHash<QString, int>* indexForms()   const { return idx_forms; }
    inline const QHash<QString, int>* indexLexemes() const { return idx_lexemes; }

    bool appendFile(const QString &fname);
    bool append    (const QString &buffer);
};

#endif // _TEXT_H_
