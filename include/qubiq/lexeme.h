#ifndef _LEXEME_H_
#define _LEXEME_H_

#include <QtCore>

// 2DO: describe arbitrary lexeme features as a free-form QHash?

class Lexeme: public QObject {
    Q_OBJECT

private:

    QString _lexeme;
    bool    _is_boundary;

    /* Each lexeme is represented in a text as a set of its forms
     * occuring in certain text positions, counted as offsets relative to
     * the first word in the text: */
    QVector<QString>  *_forms;
    QVector<ulong>    *_offsets;
    QHash<ulong, int> *_idx_offsets; // Ensure that offsets are unique

public:
    Lexeme(const QString &lexeme, bool is_boundary);
    ~Lexeme();

    inline QString lexeme()     const { return _lexeme; }
    inline bool    isBoundary() const { return _is_boundary; }
    inline bool    isVirtual()  const { return _forms->length() == 0; }

    inline const QVector<QString>* forms()   const { return _forms; }
    inline const QVector<ulong>*   offsets() const { return _offsets; }

    bool addForm(const QString &form, ulong offset, bool overwrite = false);
};

#endif // _LEXEME_H_
