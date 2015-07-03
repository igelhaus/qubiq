#ifndef _TEXT_H_
#define _TEXT_H_

#include <QtCore>
#include <cutelogger/include/Logger.h>
#include <qubiq/qubiq_global.h>
#include <qubiq/util/lexeme.h>
#include <qubiq/util/lexeme_index.h>

const qint64 DEFAULT_READ_BUFFER_SIZE = 80;

// FIXME: implement lemmatizing interface
// FIXME: tokenization with lemmatization vs. tokenization before lemmatization

class QUBIQSHARED_EXPORT Text: public QObject {
    Q_OBJECT

public:
    Text(const QLocale &locale);
    Text();
    ~Text();

    //! Returns length of the text expressed in tokens.
    //! \sa numNonBoundaries
    //! \sa numBoundaries
    //! \sa numUniqueTokens
    //! \sa numLexemes
    inline int length() const { return _offsets->length(); }

    //! Returns number of non-boundary tokens in the text.
    //! \sa length
    //! \sa numBoundaries
    //! \sa numUniqueTokens
    //! \sa numLexemes
    inline int numNonBoundaries() const { return num_forms; }

    //! Returns number of boundary tokens in the text.
    //! \sa length
    //! \sa numNonBoundaries
    //! \sa numUniqueTokens
    //! \sa numLexemes
    inline int numBoundaries() const { return num_boundaries; }

    //! Returns number of unique tokens (both boundaries and non-boundaries) in the text.
    //! \sa length
    //! \sa numNonBoundaries
    //! \sa numBoundaries
    //! \sa numLexemes
    inline int numUniqueTokens() const { return idx_forms->keys().length(); }

    //! Returns number of lexemes in the text.
    //! \sa length
    //! \sa numNonBoundaries
    //! \sa numBoundaries
    //! \sa numUniqueTokens
    inline int numLexemes() const { return _lexemes->length(); }

    //! Returns pointer to the vector of all lexemes of the text.
    //! \sa offsets
    //! \sa indexForms
    //! \sa indexLexemes
    inline const QVector<Lexeme*>* lexemes() const { return _lexemes; }

    //! Returns pointer to the vector mapping token offsets to corresponding lexemes.
    //! \sa lexemes
    //! \sa indexForms
    //! \sa indexLexemes
    inline const QVector<int>* offsets() const { return _offsets; }

    //! Returns pointer to the index mapping word forms to corresponding lexemes.
    //! \sa lexemes
    //! \sa offsets
    //! \sa indexLexemes
    inline const QHash<QString, int>* indexForms() const { return idx_forms; }

    //! Returns pointer to the index mapping string representations of lexemes
    //! to their offsets in the \c lexemes vector.
    //! \sa lexemes
    //! \sa offsets
    //! \sa indexForms
    inline const QHash<QString, int>* indexLexemes() const { return idx_lexemes; }

    // FIXME: Document me
    // FIXME: Make const?
    inline LexemeIndex* wordforms() const { return idx_wf; }

    bool appendFile(const QString &fname);
    bool appendFile(FILE *fd);
    bool append    (const QString &buffer);

private:
    QLocale _locale;
    QVector<Lexeme*>    *_lexemes;   //!< vector of all lexemes in the text
    QVector<int>        *_offsets;   //!< token's offset in the text -> offset of its lexeme in \c _lexemes
    QHash<QString, int> *idx_forms;  //!< token  (string) -> offset of its lexeme in \c _lexemes
    QHash<QString, int> *idx_lexemes;//!< lexeme (string) -> its offset in \c _lexemes
    LexemeIndex         *idx_wf;     //!< Index of word forms built on the text

    int num_forms;
    int num_boundaries;

    bool     append_file        (QFile *file);
    bool     is_boundary_token  (const QStringRef &token);
    bool     is_whitespace_token(const QStringRef &token);
    QString* normalize_token    (const QStringRef &token, bool is_boundary);
    bool     process_token      (const QStringRef &token);

    void _initialize(const QLocale &locale);
};

#endif // _TEXT_H_
