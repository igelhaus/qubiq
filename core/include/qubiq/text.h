#ifndef _TEXT_H_
#define _TEXT_H_

#include <QtCore>
#include <cutelogger/include/Logger.h>
#include <qubiq/global.h>
#include <qubiq/lexeme.h>

const qint64 DEFAULT_READ_BUFFER_SIZE = 80;

// FIXME: pass text properties: language (eventually locale setting)
// FIXME: implement lemmatizing interface
// FIXME: tokenization with lemmatization vs. tokenization before lemmatization

class QUBIQSHARED_EXPORT Text: public QObject {
    Q_OBJECT

public:
    Text();
    ~Text();

    inline int length()           const { return _offsets->length(); }
    inline int numNonBoundaries() const { return num_forms; }
    inline int numBoundaries()    const { return num_boundaries; }
    inline int numUniqueTokens()  const { return idx_forms->keys().length(); }
    inline int numLexemes()       const { return _lexemes->length(); }

    inline const QVector<Lexeme*>* lexemes()         const { return _lexemes; }
    inline const QVector<int>* offsets()             const { return _offsets; }
    inline const QHash<QString, int>* indexForms()   const { return idx_forms; }
    inline const QHash<QString, int>* indexLexemes() const { return idx_lexemes; }

    bool appendFile(const QString &fname);
    bool append    (const QString &buffer);

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

};

#endif // _TEXT_H_
