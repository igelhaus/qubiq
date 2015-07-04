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
    inline int length() const { return idx_wf->numUniquePositions(); } // FIXME: Return real value

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
    inline int numUniqueTokens() const { return idx_wf->lexemes()->keys().size(); }

    //! Returns number of lexemes in the text.
    //! \sa length
    //! \sa numNonBoundaries
    //! \sa numBoundaries
    //! \sa numUniqueTokens
    inline int numLexemes() const { return idx_lex->lexemes()->keys().size(); }

    // FIXME: Document me
    // FIXME: Make const?
    inline LexemeIndex* wordforms() const { return idx_wf; }
    inline LexemeIndex* lexmes()    const { return idx_lex; }

    bool appendFile(const QString &fname);
    bool appendFile(FILE *fd);
    bool append    (const QString &buffer);

private:
    QLocale _locale;
    LexemeIndex *idx_wf;  //!< Index of word forms built on the text
    LexemeIndex *idx_lex; //!< Index of lexemes built on the text

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
