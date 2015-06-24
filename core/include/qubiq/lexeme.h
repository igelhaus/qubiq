#ifndef _LEXEME_H_
#define _LEXEME_H_

#include <QtCore>
#include <qubiq/global.h>

// 2DO: describe arbitrary lexeme features as a free-form QHash?

class QUBIQSHARED_EXPORT Lexeme {

private:

    QString _lexeme;
    bool    _is_boundary;

    /* Each lexeme is represented in a text as a set of its forms
     * occuring in certain text positions, counted as offsets relative to
     * the first word in the text: */
    QVector<QString> *_forms;
    QVector<int>     *_offsets;
    QHash<int, int>  *_idx_offsets; // Ensure that offsets are unique

    void _initialize(const QString &name, bool is_boundary);
    void _destroy();
    void _assign(const Lexeme &other);

public:
    Lexeme(const QString &name);
    Lexeme(const Lexeme &other);
    ~Lexeme();

    Lexeme(const QString &lexeme, bool is_boundary); // FIXME: Remove this one

    Lexeme &operator =(const Lexeme &other);

    inline QString lexeme()     const { return _lexeme; }
    inline QString name()       const { return _lexeme; }
    inline bool    isBoundary() const { return _is_boundary; }
    inline bool    isVirtual()  const { return _forms->length() == 0; }

    inline const QVector<QString>* forms() const { return _forms; }
    inline const QVector<int>*   offsets() const { return _offsets; }

    inline void setIsBoundary(bool is_boundary) { _is_boundary = is_boundary; }

    bool addForm(const QString &form, int offset, bool overwrite = false);
};

#endif // _LEXEME_H_
