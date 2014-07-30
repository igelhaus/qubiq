#include <QtTest/QtTest>

#include <qubiq/lexeme_sequence.h>

class TestLexemeSequence: public QObject
{
    Q_OBJECT

private slots:
    void emptySequence();
};

void TestLexemeSequence::emptySequence()
{
    LexemeSequence sequence;
    QCOMPARE(sequence.isValid(), false);
}

QTEST_MAIN(TestLexemeSequence)
#include "test_lexeme_sequence.moc"
