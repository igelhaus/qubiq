#include <qubiq/text.h>

/**
 * \class Text
 *
 * \brief The Text class provides means for indexing a text before further processing.
 *
 * \sa LexemeSequence
 */

//! Constructs a Text object in the "C" locale.
Text::Text()
{
    _initialize(QLocale("C"));
}

//! This is an overloaded constructor. Constructs a Text object in the given locale.
Text::Text(const QLocale &locale) {
    _initialize(locale);
}

//! Destructs the Text object.
Text::~Text() {
    delete idx_wf;
    delete idx_lex;
}

/**
 * Appends contents of a file referenced by its name to the text.
 *
 * \param[in] fname Name of the file to append to the text.
 *
 * \returns \c true on success and \c false if the file is not accessible.
 *
 * \note
 * The method will return \c true on empty files and files containing whitespace
 * characters only.
 */
bool Text::appendFile(const QString &fname)
{
    LOG_INFO() << "Starting indexing file" << fname;

    QFile file(fname);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING("Unable to access file");
        return false;
    }

    return append_file(&file);
}

/**
 * This is an overloaded function.
 *
 * Appends contents of a file referenced by its descriptor to the text.
 *
 * \param[in] fd Descriptor of the file to append to the text.
 *
 * \returns \c true on success and \c false if the file is not accessible.
 *
 * \note
 * The method will return \c true on empty files and files containing whitespace
 * characters only.
 */
bool Text::appendFile(FILE *fd)
{
    LOG_INFO() << "Starting indexing a file descriptor";

    QFile file;
    if (!file.open(fd, QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING("Unable to access file by file descriptor");
        return false;
    }

    return append_file(&file);
}

bool Text::append_file(QFile *file)
{
    QTextStream file_stream(file);
    QString     buffer = file_stream.read(DEFAULT_READ_BUFFER_SIZE);
    QString     token_part;
    while (buffer.length() > 0) {
        QTextBoundaryFinder *boundary_finder = new QTextBoundaryFinder(
            QTextBoundaryFinder::Word, buffer
        );
        int pos_start = boundary_finder->position();
        int pos_end   = boundary_finder->toNextBoundary();
        while (pos_end != -1) {
            QStringRef token = buffer.midRef(pos_start, pos_end - pos_start);
            pos_start = pos_end;
            pos_end   = boundary_finder->toNextBoundary();
            if (pos_end != -1) {
                LOG_DEBUG() << "token =" << token;
                process_token(token);
            } else {
                token_part = token.toString();
                LOG_DEBUG() << "token_part =" << token_part;
            }
        };
        delete boundary_finder;

        buffer.clear();
        QString _buffer = file_stream.read(DEFAULT_READ_BUFFER_SIZE);
        if (!_buffer.isNull()) {
            buffer.append(token_part).append(_buffer);
        } else {
            /* Last chunk is guaranteed to be a valid token, process it: */
            LOG_DEBUG() << "very_last_token =" << (token_part);
            QStringRef token = token_part.midRef(0, token_part.length());
            process_token(token);
        }
    }

    LOG_INFO("File indexed");

    return true;
}

/**
 * Appends contents of a string buffer to the text.
 *
 * \param[in] buffer Buffer to append to the text.
 *
 * \returns \c true on success and \c false if the buffer is a null/empty string.
 */
bool Text::append(const QString &buffer)
{
    if (buffer.isEmpty() || buffer.isNull())
        return false;

    QTextBoundaryFinder *boundary_finder = new QTextBoundaryFinder(
        QTextBoundaryFinder::Word, buffer
    );
    int pos_start = boundary_finder->position();
    int pos_end   = boundary_finder->toNextBoundary();
    while (pos_end != -1) {
        QStringRef token = buffer.midRef(pos_start, pos_end - pos_start);
        pos_start = pos_end;
        pos_end   = boundary_finder->toNextBoundary();
        LOG_DEBUG() << "token =" << token;
        process_token(token);
    };
    delete boundary_finder;
    return true;
}

/**
 * Detects whether a token consists of whitespace characters only.
 *
 * \param[in] token Token to be checked.
 *
 * \returns \c true if a token consists of whitespace characters only and \c false otherwise.
 */
bool Text::is_whitespace_token(const QStringRef &token)
{
    const QChar *chars = token.unicode();
    for (int i = 0; i < token.length(); i++) {
        if (!chars[i].isSpace())
            return false;
    }
    return true;
}

/**
 * Detects whether a token is a boundary token.
 *
 * \param[in] token Token to be checked.
 *
 * \returns \c true if a token is a boundary token and \c false otherwise.
 */
bool Text::is_boundary_token(const QStringRef &token)
{
    const QChar *chars = token.unicode();
    for (int i = 0; i < token.length(); i++) {
        bool is_punct_char = chars[i].isPunct()
            ||  chars[i] < 0x30                     // includes chars like '*', '+', etc.
            || (chars[i] > 0x39 && chars[i] < 0x41) // includes chars like '<', '>', etc.
            || (chars[i] > 0x5A && chars[i] < 0x61) // includes chars like '^', '`', etc.
            || (chars[i] > 0x7A && chars[i] < 0x7F) // includes chars like '|', '~', etc.
        ;
        if (!is_punct_char)
            return false;
    }
    return true;
}

QString* Text::normalize_token(const QStringRef &token, bool is_boundary)
{
    QString *normalized;

    if (is_boundary) {
        normalized = new QString(token.toString());
    } else {
        // 2DO: add an entry point morphology routines
        normalized = new QString(_locale.toLower(token.toString()));
    }

    return normalized;
}

/**
 * Adds a token to the text indeces.
 *
 * All whitespace tokens are ignored. If the token is lemmatized to a new lexeme
 * it is inserted into the index of lexemes.
 *
 * \param[in] token Token to process.
 *
 * \returns \c true if a token consists of whitespace characters only and \c false otherwise.
 */
bool Text::process_token(const QStringRef &token)
{
    if (is_whitespace_token(token))
        return false;

    QString token_key = _locale.toLower(token.toString());
    int  pos          = idx_wf->numUniquePositions();
    bool is_new       = false;

    Lexeme *lexeme = idx_wf->addPosition(token_key, pos, &is_new);
    if (is_new == true) {
        // FIXME: Add other universal properties (is_number etc.)
        bool is_boundary = is_boundary_token(token);
        lexeme->setIsBoundary(is_boundary);
    }

    return true;
}

//! \internal Initializes class members.
void Text::_initialize(const QLocale &locale)
{
    _locale = locale;
    idx_wf  = new LexemeIndex();
    idx_lex = new LexemeIndex();
}
