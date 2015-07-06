#include <QtTest/QtTest>
#include <qubiq/util/lexeme_index.h>

class TestLexemeIndex: public QObject
{
    Q_OBJECT

private slots:
    void emptyIndex();
    void addPosition();
    void addPositions();
    void mergeIndeces();
    void copyFromIndex();
};

void TestLexemeIndex::emptyIndex()
{
    LexemeIndex index;

    QCOMPARE(index.size(), 0);
    QCOMPARE(index.findByName("")     == NULL, true);
    QCOMPARE(index.findByPosition(0)  == NULL, true);
    QCOMPARE(index.findByPosition(-1) == NULL, true);
}

void TestLexemeIndex::addPosition()
{
    // Consider an index of wordforms on the text:
    // 0 1   2   3 4
    // a man saw a man
    LexemeIndex index;
    bool is_new;

    Lexeme *l0 = index.addPosition("a",   0, &is_new);
    QCOMPARE(is_new, true);

    Lexeme *l1 = index.addPosition("man", 1, &is_new);
    QCOMPARE(is_new, true);

    Lexeme *l2 = index.addPosition("saw", 2, &is_new);
    QCOMPARE(is_new, true);

    Lexeme *l3 = index.addPosition("a",   3, &is_new);
    QCOMPARE(is_new, false);

    Lexeme *l4 = index.addPosition("man", 4, &is_new);
    QCOMPARE(is_new, false);

    QCOMPARE(index.size(), 3);

    QCOMPARE(l0 == l3, true);
    QCOMPARE(l1 == l4, true);
    QCOMPARE(l0 != l2, true);
    QCOMPARE(l4 != l2, true);

    QCOMPARE(index.findByPosition(0) == l0, true);
    QCOMPARE(index.findByPosition(1) == l1, true);
    QCOMPARE(index.findByPosition(2) == l2, true);
    QCOMPARE(index.findByPosition(3) == l3, true);
    QCOMPARE(index.findByPosition(4) == l4, true);

    QCOMPARE(index.findByPosition(-1) == NULL, true);
    QCOMPARE(index.findByPosition( 5) == NULL, true);

    QCOMPARE(index.findByName("a")   == l0, true);
    QCOMPARE(index.findByName("man") == l1, true);
    QCOMPARE(index.findByName("saw") == l2, true);

    QCOMPARE(index.findByName("")    == NULL, true);
    QCOMPARE(index.findByName("see") == NULL, true);

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
        << "a"   << "man" << "wants" << "to"    << "see" << "the" // index1
        << "man" << "men" << "will"  << "never" << "see"          // index2
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

    QCOMPARE(index1.size(), 6);
    QCOMPARE(index2.size(), 5);
    QCOMPARE(index1.numUniquePositions(), 6);
    QCOMPARE(index2.numUniquePositions(), 5);

    QCOMPARE(index1.findByName("wants") != NULL, true);
    QCOMPARE(index1.findByName("will")  == NULL, true);
    QCOMPARE(index1.findByName("never") == NULL, true);
    QCOMPARE(index2.findByName("wants") == NULL, true);
    QCOMPARE(index2.findByName("will")  != NULL, true);
    QCOMPARE(index2.findByName("never") != NULL, true);

    index1.merge(index2);

    QCOMPARE(index1.size(), 9);
    QCOMPARE(index2.size(), 5);
    QCOMPARE(index1.numUniquePositions(), 11);
    QCOMPARE(index2.numUniquePositions(),  5);

    QCOMPARE(index1.findByName("wants") != NULL, true);
    QCOMPARE(index1.findByName("will")  != NULL, true);
    QCOMPARE(index1.findByName("never") != NULL, true);
    QCOMPARE(index2.findByName("wants") == NULL, true);
    QCOMPARE(index2.findByName("will")  != NULL, true);
    QCOMPARE(index2.findByName("never") != NULL, true);

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
        QCOMPARE(index1.findByPosition(i) != NULL, true);
    }
    // ...while index2 is still partial:
    for (int i = 0; i < wordforms.size(); i++) {
        QCOMPARE(index2.findByPosition(i) == NULL, i < 6);
    }
}

void TestLexemeIndex::copyFromIndex()
{
    // Consider indeces of wordforms on the text:
    // 0 1   2     3  4   5   6   7   8    9     10
    // a man wants to see the man men will never see
    QStringList wordforms;
    wordforms
        << "a"   << "man" << "wants" << "to"    << "see" << "the" // index1
        << "man" << "men" << "will"  << "never" << "see"          // index2
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
    QCOMPARE(index1.numUniquePositions(), 6);
    QCOMPARE(index2.numUniquePositions(), 5);

    bool is_new;
    Lexeme *src;
    Lexeme *dst;

    src = index1.findByName("to");
    dst = index2.copyFromIndex(index1, "to", &is_new);

    QCOMPARE(is_new, true);
    QCOMPARE(src != dst, true);

    QCOMPARE(index1.numUniquePositions(), 6);
    QCOMPARE(index2.numUniquePositions(), 6);

    QCOMPARE(index1.positions("to") != index2.positions("to"), true);
    QCOMPARE(index1.positions("to")->size(), 1);
    QCOMPARE(index2.positions("to")->size(), 1);
    QCOMPARE(index1.positions("to")->at(0), index2.positions("to")->at(0));

    Lexeme *index2_see = index2.findByName("see");

    src = index1.findByName("see");
    dst = index2.copyFromIndex(index1, "see", &is_new);

    QCOMPARE(is_new, false);
    QCOMPARE(src != dst       , true);
    QCOMPARE(dst == index2_see, true);

    QCOMPARE(index1.numUniquePositions(), 6);
    QCOMPARE(index2.numUniquePositions(), 7);

    QCOMPARE(index1.positions("see")->size(), 1);
    QCOMPARE(index2.positions("see")->size(), 2);
    QCOMPARE(index1.positions("see")->at(0),  4);
    QCOMPARE(index2.positions("see")->at(0), 10);
    QCOMPARE(index2.positions("see")->at(1),  4);
}

QTEST_MAIN(TestLexemeIndex)
#include "test_lexeme_index.moc"
