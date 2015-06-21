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
        delete (*l2p);
        *l2p = NULL;
    }
    delete lex2pos;

    QHash<QString, Lexeme*>::iterator l = lex->begin();
    while (l != lex->end()) {
        delete (*l);
        *l = NULL;
    }
    delete lex;
}

Lexeme* LexemeIndex::lexemeByName(const QString &name)
{
    return lex->value(name, NULL);
}

Lexeme* LexemeIndex::lexemeByPosition(int pos)
{
    return pos2lex->value(pos, NULL);
}

QVector<int>* LexemeIndex::positions(const QString &name)
{
    return lex2pos->value(name, NULL);
}

QHash<QString, Lexeme*>* LexemeIndex::lexemes()
{
    return lex;
}

bool LexemeIndex::add(const QString &name, bool is_boundary, int pos)
{
    if (pos < 0)
        return false;

    if (!lex->contains(name)) {
        init_entry(name, is_boundary);
    }
    Lexeme       *lexeme    = lex->value(name);
    QVector<int> *positions = lex2pos->value(name);

    pos2lex->insert(pos, lexeme);
    positions->append(pos);

    return true;
}

bool LexemeIndex::add(const QString &name, bool is_boundary, QVector<int> *pos)
{
    if (pos == NULL)
        return false;

    if (!lex->contains(name)) {
        init_entry(name, is_boundary);
    }
    Lexeme       *lexeme    = lex->value(name);
    QVector<int> *positions = lex2pos->value(name);

    for (int i = 0; i < pos->size(); i++) {
        int _pos = pos->at(i);
        pos2lex->insert(_pos, lexeme);
        positions->append(_pos);
    }

    return true;
}

void LexemeIndex::merge(LexemeIndex *other)
{
    QHash<QString, Lexeme*> *lexemes = other->lexemes();
    QHash<QString, Lexeme*>::const_iterator it_l;
    for (it_l = lexemes->constBegin(); it_l != lexemes->constEnd(); ++it_l) {
        Lexeme *lexeme = *it_l;
        this->add(
            lexeme->lexeme(),
            lexeme->isBoundary(),
            other->positions(lexeme->lexeme())
        );
    }
}

void LexemeIndex::init_entry(const QString &name, bool is_boundary)
{
    Lexeme *lexeme          = new Lexeme(name, is_boundary);
    QVector<int> *positions = new QVector<int>;
    lex->insert(name, lexeme);
    lex2pos->insert(name, positions);
}
