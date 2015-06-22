#include <qubiq/lexeme_index.h>

LexemeIndex::LexemeIndex()
{
    lex     = new QHash<QString, Lexeme*>;
    lex2pos = new QHash<QString, QVector<int>*>;
    pos2lex = new QHash<int, Lexeme*>;
}

LexemeIndex::~LexemeIndex()
{
    delete pos2lex;

    QHash<QString, QVector<int>*>::iterator l2p = lex2pos->begin();
    while (l2p != lex2pos->end()) {
        delete *l2p;
        l2p = lex2pos->erase(l2p);
    }
    delete lex2pos;

    QHash<QString, Lexeme*>::iterator l = lex->begin();
    while (l != lex->end()) {
        delete *l;
        l = lex->erase(l);
    }
    delete lex;
}

Lexeme* LexemeIndex::addPosition(const QString &name, int pos)
{
    if (pos < 0)
        return NULL;

    if (!lex->contains(name)) {
        init_entry(name);
    }
    Lexeme       *lexeme    = lex->value(name);
    QVector<int> *positions = lex2pos->value(name);

    pos2lex->insert(pos, lexeme);
    positions->append(pos);

    return lexeme;
}

Lexeme* LexemeIndex::addPositions(const QString &name, const QVector<int> *pos)
{
    if (pos == NULL)
        return NULL;

    if (!lex->contains(name)) {
        init_entry(name);
    }
    Lexeme       *lexeme    = lex->value(name);
    QVector<int> *positions = lex2pos->value(name);

    for (int i = 0; i < pos->size(); i++) {
        int _pos = pos->at(i);
        pos2lex->insert(_pos, lexeme);
        positions->append(_pos);
    }

    return lexeme;
}

void LexemeIndex::merge(const LexemeIndex &other)
{
    QHash<QString, Lexeme*> *lexemes = other.lexemes();
    QHash<QString, Lexeme*>::const_iterator it_l;
    for (it_l = lexemes->constBegin(); it_l != lexemes->constEnd(); ++it_l) {
        Lexeme *lexeme = *it_l;
        addPositions(lexeme->lexeme(), other.positions(lexeme->lexeme()));
    }
}

void LexemeIndex::init_entry(const QString &name)
{
    Lexeme *lexeme          = new Lexeme(name, false); // FIXME: Adjust Lexeme class
    QVector<int> *positions = new QVector<int>;
    lex->insert(name, lexeme);
    lex2pos->insert(name, positions);
}
