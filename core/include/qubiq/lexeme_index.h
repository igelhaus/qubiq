#ifndef _LEXEME_INDEX_H_
#define _LEXEME_INDEX_H_

#include <QtCore>
#include <qubiq/lexeme.h>

class LexemeIndex {

public:
    LexemeIndex();
    ~LexemeIndex();

    inline QHash<QString, Lexeme*>* lexemes() const { return lex; }

    inline Lexeme* lexemeByPosition(int pos) const { return pos2lex->value(pos, NULL); }
    inline Lexeme* lexemeByName(const QString &name) const { return lex->value(name, NULL); }
    inline QVector<int>* positions(const QString &name) const { return lex2pos->value(name, NULL); }

    bool add(const QString &name, bool is_boundary, int pos);
    bool add(const QString &name, bool is_boundary, const QVector<int> *pos);
    void merge(const LexemeIndex &other);

private:
    QHash<QString, Lexeme*>       *lex;
    QHash<QString, QVector<int>*> *lex2pos;
    QHash<int, Lexeme*>           *pos2lex;

    void init_entry(const QString &name, bool is_boundary);
};

#endif // _LEXEME_INDEX_H_
