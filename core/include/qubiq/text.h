#ifndef _TEXT_H_
#define _TEXT_H_

#include <QtCore>
#include <cutelogger/include/Logger.h>
#include <qubiq/qubiq_global.h>
#include <qubiq/util/lexeme.h>
#include <qubiq/util/lexeme_index.h>

const qint64 DEFAULT_READ_BUFFER_SIZE = 80;

class QUBIQSHARED_EXPORT Text: public QObject {
    Q_OBJECT

public:
    Text(const QLocale &locale);
    Text();
    ~Text();

    //! Returns length of the text expressed in tokens.
    inline int length() const { return idx_wf->numUniquePositions(); }

    //! Returns a pointer to the index of wordforms assosiated with the text.
    //! \sa lexemes
    inline LexemeIndex* wordforms() const { return idx_wf; }

    //! Returns a pointer to the index of lexemes assosiated with the text.
    //! \sa wordforms
    inline LexemeIndex* lexmes() const { return idx_lex; }

    bool appendFile(const QString &fname);
    bool appendFile(FILE *fd);
    bool append    (const QString &buffer);

private:
    QLocale _locale;

    LexemeIndex *idx_wf;  //!< Index of word forms built on the text
    LexemeIndex *idx_lex; //!< Index of lexemes built on the text

    bool     append_file        (QFile *file);
    bool     is_boundary_token  (const QStringRef &token);
    bool     is_whitespace_token(const QStringRef &token);
    QString* normalize_token    (const QStringRef &token, bool is_boundary);
    bool     process_token      (const QStringRef &token);

    void _initialize(const QLocale &locale);
};

#endif // _TEXT_H_
