#ifndef _LEXEME_INDEX_H_
#define _LEXEME_INDEX_H_

#include <QtCore>
#include <qubiq/global.h>
#include <qubiq/lexeme.h>

class QUBIQSHARED_EXPORT LexemeIndex {

public:
    LexemeIndex();
    ~LexemeIndex();

    inline QHash<QString, Lexeme*>* lexemes() const { return lex; }

    inline Lexeme* findByPosition(int pos) const { return pos2lex->value(pos, NULL); }
    inline Lexeme* findByName(const QString &name) const { return lex->value(name, NULL); }
    inline QVector<int>* positions(const QString &name) const { return lex2pos->value(name, NULL); }

    Lexeme* addPosition(const QString &name, int pos, bool *is_new = NULL);
    Lexeme* addPositions(const QString &name, const QVector<int> *pos, bool *is_new = NULL);

    Lexeme* copyFromIndex(const LexemeIndex &other, const QString &name, bool *is_new = NULL); // FIXME: Cover with tests

    void merge(const LexemeIndex &other);

private:
    QHash<QString, Lexeme*>       *lex;
    QHash<QString, QVector<int>*> *lex2pos;
    QHash<int, Lexeme*>           *pos2lex;

    void init_entry(const QString &name, bool *is_new);
};

#endif // _LEXEME_INDEX_H_
