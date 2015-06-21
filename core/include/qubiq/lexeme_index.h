#ifndef _LEXEME_INDEX_H_
#define _LEXEME_INDEX_H_

#include <QtCore>
#include <qubiq/lexeme.h>

class LexemeIndex {

public:
    LexemeIndex();
    ~LexemeIndex();

    Lexeme* lexemeByPosition(int pos);
    Lexeme* lexemeByName(const QString &name);
    QVector<int>* positions(const QString &name);

    QHash<QString, Lexeme*>* lexemes();

    bool add(const QString &name, bool is_boundary, int pos);
    bool add(const QString &name, bool is_boundary, QVector<int> *pos);
    void merge(LexemeIndex *other);

private:
    QHash<QString, Lexeme*>       *lex;
    QHash<QString, QVector<int>*> *lex2pos;
    QHash<int, Lexeme*>           *pos2lex;

    void init_entry(const QString &name, bool is_boundary);
};

#endif // _LEXEME_INDEX_H_
