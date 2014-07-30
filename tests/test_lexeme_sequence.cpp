#include <QtTest/QtTest>

#include <qubiq/lexeme_sequence.h>

// FIXME: test non-empty sequences
// FIXME: test hashing

class TestLexemeSequence: public QObject
{
    Q_OBJECT

private slots:
    void emptySequence();
    void emptySequenceCopy();
    void emptySequenceAssignment();
};

void TestLexemeSequence::emptySequence()
{
    LexemeSequence sequence;
    QCOMPARE(sequence.state(), LexemeSequence::STATE_EMPTY);
    QCOMPARE(sequence.isValid(), false);
    QCOMPARE(sequence.length(), (ulong)0);
    QCOMPARE(sequence.boundary(), (ulong)0);
    QCOMPARE(sequence.lexemes()->length(), 0);
    QCOMPARE(sequence.sequenceKey()->length(), 0);
    QCOMPARE(sequence.mi(), 0.0);
    QCOMPARE(sequence.llr(), 0.0);
    QCOMPARE(sequence.score(), 0.0);
}

void TestLexemeSequence::emptySequenceCopy()
{
    LexemeSequence sequence1;
    LexemeSequence sequence2(sequence1);

    QCOMPARE(sequence2.state(), LexemeSequence::STATE_EMPTY);
    QCOMPARE(sequence2.isValid(), false);
    QCOMPARE(sequence2.length(), (ulong)0);
    QCOMPARE(sequence2.boundary(), (ulong)0);
    QCOMPARE(sequence2.lexemes()->length(), 0);
    QCOMPARE(sequence2.sequenceKey()->length(), 0);
    QCOMPARE(sequence2.mi(), 0.0);
    QCOMPARE(sequence2.llr(), 0.0);
    QCOMPARE(sequence2.score(), 0.0);
}

void TestLexemeSequence::emptySequenceAssignment()
{
    LexemeSequence sequence1;
    LexemeSequence sequence2;

    sequence2 = sequence1;

    QCOMPARE(sequence2.state(), LexemeSequence::STATE_EMPTY);
    QCOMPARE(sequence2.isValid(), false);
    QCOMPARE(sequence2.length(), (ulong)0);
    QCOMPARE(sequence2.boundary(), (ulong)0);
    QCOMPARE(sequence2.lexemes()->length(), 0);
    QCOMPARE(sequence2.sequenceKey()->length(), 0);
    QCOMPARE(sequence2.mi(), 0.0);
    QCOMPARE(sequence2.llr(), 0.0);
    QCOMPARE(sequence2.score(), 0.0);
}

QTEST_MAIN(TestLexemeSequence)
#include "test_lexeme_sequence.moc"
