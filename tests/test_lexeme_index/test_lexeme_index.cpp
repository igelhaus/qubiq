#include <QtTest/QtTest>
#include <qubiq/lexeme_index.h>

class TestLexemeIndex: public QObject
{
    Q_OBJECT

private slots:
    void emptyIndex();
    void addPosition();
    void addPositions();
    void mergeIndeces();
};

void TestLexemeIndex::emptyIndex()
{
    LexemeIndex index;

    QCOMPARE(index.lexemes()->keys().size(), 0);
    QCOMPARE(index.lexemeByName("")     == NULL, true);
    QCOMPARE(index.lexemeByPosition(0)  == NULL, true);
    QCOMPARE(index.lexemeByPosition(-1) == NULL, true);
}

void TestLexemeIndex::addPosition()
{
    // Consider an index of wordforms on the text:
    // 0 1   2   3 4
    // a man saw a man
    LexemeIndex index;

    Lexeme *l0 = index.addPosition("a",   0);
    Lexeme *l1 = index.addPosition("man", 1);
    Lexeme *l2 = index.addPosition("saw", 2);
    Lexeme *l3 = index.addPosition("a",   3);
    Lexeme *l4 = index.addPosition("man", 4);

    QCOMPARE(l0 == l3, true);
    QCOMPARE(l1 == l4, true);
    QCOMPARE(l0 != l2, true);
    QCOMPARE(l4 != l2, true);

    QCOMPARE(index.lexemeByPosition(0) == l0, true);
    QCOMPARE(index.lexemeByPosition(1) == l1, true);
    QCOMPARE(index.lexemeByPosition(2) == l2, true);
    QCOMPARE(index.lexemeByPosition(3) == l3, true);
    QCOMPARE(index.lexemeByPosition(4) == l4, true);

    QCOMPARE(index.lexemeByPosition(-1) == NULL, true);
    QCOMPARE(index.lexemeByPosition( 5) == NULL, true);

    QCOMPARE(index.lexemeByName("a")   == l0, true);
    QCOMPARE(index.lexemeByName("man") == l1, true);
    QCOMPARE(index.lexemeByName("saw") == l2, true);

    QCOMPARE(index.lexemeByName("")    == NULL, true);
    QCOMPARE(index.lexemeByName("see") == NULL, true);

    QVector<int>* pos0 = index.positions("a");
    QCOMPARE(pos0->size(), 2);
    QCOMPARE(pos0->at(0), 0);
    QCOMPARE(pos0->at(1), 3);

    QVector<int>* pos1 = index.positions("man");
    QCOMPARE(pos1->size(), 2);
    QCOMPARE(pos1->at(0), 1);
    QCOMPARE(pos1->at(1), 4);

    QVector<int>* pos2 = index.positions("saw");
    QCOMPARE(pos2->size(), 1);
    QCOMPARE(pos2->at(0), 2);
}

void TestLexemeIndex::addPositions()
{
    // Consider an index of true lexemes on the text:
    // 0 1   2     3  4   5   6   7   8    9     10
    // a man wants to see the man men have never seen
    LexemeIndex index;

    QVector<int> *pos_MAN1 = new QVector<int>();
    *pos_MAN1 << 1 << 6;

    QVector<int> *pos_SEE = new QVector<int>();
    *pos_SEE << 4 << 10;

    Lexeme *lex_MAN1 = index.addPositions("man", pos_MAN1);
    Lexeme *lex_SEE  = index.addPositions("see", pos_SEE);

    QCOMPARE(lex_MAN1 != lex_SEE, true);
    QCOMPARE(index.positions("man")->size(),  2);
    QCOMPARE(index.positions("man")->at(0) ,  1);
    QCOMPARE(index.positions("man")->at(1) ,  6);
    QCOMPARE(index.positions("see")->size(),  2);
    QCOMPARE(index.positions("see")->at(0) ,  4);
    QCOMPARE(index.positions("see")->at(1) , 10);

    QVector<int> *pos_MAN2 = new QVector<int>();
    *pos_MAN2 << 7;

    Lexeme *lex_MAN2 = index.addPositions("man", pos_MAN2);
    QCOMPARE(lex_MAN1 == lex_MAN2, true);
    QCOMPARE(index.positions("man")->size(),  3);
    QCOMPARE(index.positions("man")->at(0) ,  1);
    QCOMPARE(index.positions("man")->at(1) ,  6);
    QCOMPARE(index.positions("man")->at(2) ,  7);

    delete pos_MAN1;
    delete pos_MAN2;
    delete pos_SEE;
}

void TestLexemeIndex::mergeIndeces()
{
    // Consider indeces of wordforms on the text:
    // 0 1   2     3  4   5   6   7   8    9     10
    // a man wants to see the man men will never see
    QStringList wordforms;
    wordforms
        << "a"   << "man"  << "wants" << "to" << "see" << "the" << "man"
        << "men" << "will" << "never" << "see"
    ;

    LexemeIndex index1;
    LexemeIndex index2;
    for (int i = 0; i < wordforms.size(); i++) {
        if (i < 6) {
            index1.addPosition(wordforms.at(i), i);
        } else {
            index2.addPosition(wordforms.at(i), i);
        }
    }

    QCOMPARE(index1.lexemeByName("wants") != NULL, true);
    QCOMPARE(index1.lexemeByName("will")  == NULL, true);
    QCOMPARE(index1.lexemeByName("never") == NULL, true);
    QCOMPARE(index2.lexemeByName("wants") == NULL, true);
    QCOMPARE(index2.lexemeByName("will")  != NULL, true);
    QCOMPARE(index2.lexemeByName("never") != NULL, true);

    index1.merge(index2);

    QCOMPARE(index1.lexemeByName("wants") != NULL, true);
    QCOMPARE(index1.lexemeByName("will")  != NULL, true);
    QCOMPARE(index1.lexemeByName("never") != NULL, true);
    QCOMPARE(index2.lexemeByName("wants") == NULL, true);
    QCOMPARE(index2.lexemeByName("will")  != NULL, true);
    QCOMPARE(index2.lexemeByName("never") != NULL, true);

    QCOMPARE(index1.positions("man")->size(), 2);
    QCOMPARE(index1.positions("man")->at(0) , 1);
    QCOMPARE(index1.positions("man")->at(1) , 6);

    QCOMPARE(index1.positions("see")->size(),  2);
    QCOMPARE(index1.positions("see")->at(0) ,  4);
    QCOMPARE(index1.positions("see")->at(1) , 10);

    QCOMPARE(index1.positions("never")->size(), 1);
    QCOMPARE(index1.positions("never")->at(0) , 9);

    // After merge, index1 covers the whole text...
    for (int i = 0; i < wordforms.size(); i++) {
        QCOMPARE(index1.lexemeByPosition(i) != NULL, true);
    }
    // ...while index2 is still partial:
    for (int i = 0; i < wordforms.size(); i++) {
        QCOMPARE(index2.lexemeByPosition(i) == NULL, i < 6);
    }
}

QTEST_MAIN(TestLexemeIndex)
#include "test_lexeme_index.moc"
