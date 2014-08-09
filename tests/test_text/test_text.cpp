#include <QtTest/QtTest>

#include <qubiq/text.h>

class TestText: public QObject
{
    Q_OBJECT

private slots:
    void emptyText();
    void simpleSentence();
    void simpleSentenceFromFile();
    void longSentenceFromFile();
    void appendFromNonExistentFile();
};

void TestText::emptyText()
{
    Text text;

    QCOMPARE(text.length(), 0);
    QCOMPARE(text.numNonBoundaries(), 0);
    QCOMPARE(text.numBoundaries(), 0);
    QCOMPARE(text.numUniqueTokens(), 0);
    QCOMPARE(text.numLexemes(), 0);
}

void TestText::simpleSentence()
{
    Text text;

    QCOMPARE(text.append(QString(
       "The quick brown fox jumps over the lazy dog."
    )), true);

    QCOMPARE(text.length(),          10);
    QCOMPARE(text.numNonBoundaries(), 9);
    QCOMPARE(text.numBoundaries(),    1);
    QCOMPARE(text.numUniqueTokens(),  9);
    QCOMPARE(text.numLexemes(),       9);

    QCOMPARE(text.offsets()->at(0), 0);
    QCOMPARE(text.offsets()->at(6), 0);
    QCOMPARE(text.lexemes()->at(0)->offsets()->length(), 2);
    QCOMPARE(text.indexLexemes()->value("the"), 0);
    QCOMPARE(text.indexLexemes()->value("jumps"), 4);
    QCOMPARE(text.indexLexemes()->value("."), 8);
    QCOMPARE(text.indexForms()->value("the"), 0);
    QCOMPARE(text.indexForms()->value("jumps"), 4);
    QCOMPARE(text.indexForms()->value("."), 8);
}

void TestText::simpleSentenceFromFile()
{
    QTemporaryFile text_file;
    text_file.open();
    text_file.write("The quick brown fox\n");
    text_file.write("jumps over the lazy dog.");
    text_file.close();

    Text text;
    QCOMPARE(text.appendFile(text_file.fileName()), true);

    QCOMPARE(text.length(),          10);
    QCOMPARE(text.numNonBoundaries(), 9);
    QCOMPARE(text.numBoundaries(),    1);
    QCOMPARE(text.numUniqueTokens(),  9);
    QCOMPARE(text.numLexemes(),       9);

    QCOMPARE(text.offsets()->at(0), 0);
    QCOMPARE(text.offsets()->at(6), 0);
    QCOMPARE(text.lexemes()->at(0)->offsets()->length(), 2);
    QCOMPARE(text.indexLexemes()->value("the"), 0);
    QCOMPARE(text.indexLexemes()->value("jumps"), 4);
    QCOMPARE(text.indexLexemes()->value("."), 8);
    QCOMPARE(text.indexForms()->value("the"), 0);
    QCOMPARE(text.indexForms()->value("jumps"), 4);
    QCOMPARE(text.indexForms()->value("."), 8);
}

void TestText::longSentenceFromFile()
{
    // NB! This test depends on DEFAULT_READ_BUFFER_SIZE value
    QTemporaryFile text_file;
    text_file.open();
    text_file.write("The quick brown fox jumps over the laaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaazy dog.");
    text_file.close();

    Text text;
    QCOMPARE(text.appendFile(text_file.fileName()), true);

    QCOMPARE(text.length(),          10);
    QCOMPARE(text.numNonBoundaries(), 9);
    QCOMPARE(text.numBoundaries(),    1);
    QCOMPARE(text.numUniqueTokens(),  9);
    QCOMPARE(text.numLexemes(),       9);
}

void TestText::appendFromNonExistentFile()
{
    Text text;
    QCOMPARE(text.appendFile("non-existent.txt"), false);
}

QTEST_MAIN(TestText)
#include "test_text.moc"
