#include <QtTest/QtTest>

#include <qubiq/lexeme_sequence.h>

// FIXME: test non-empty sequences
// FIXME: test hashing function
// FIXME: test hashes and sets

class TestLexemeSequence: public QObject
{
    Q_OBJECT

private slots:
    void emptySequence();
    void emptySequenceCopy();
    void emptySequenceAssignment();
    void badSequenceStates();
    void simpleSequence();
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

void TestLexemeSequence::badSequenceStates()
{
    Text text;

    text.append(QString("The quick brown fox jumps over the lazy dog."));

    // FIXME: rename boundary to n1? left_len?

    LexemeSequence sequence1(&text, 1, 1, 1);
    QCOMPARE(sequence1.state(), LexemeSequence::STATE_UNIGRAM);

    LexemeSequence sequence2(&text, 1, 2, 0);
    QCOMPARE(sequence2.state(), LexemeSequence::STATE_BAD_BOUNDARY);

    LexemeSequence sequence3(&text, 1, 2, 2);
    QCOMPARE(sequence3.state(), LexemeSequence::STATE_BAD_BOUNDARY);

    LexemeSequence sequence4(&text, 10, 2, 1);
    QCOMPARE(sequence4.state(), LexemeSequence::STATE_BAD_OFFSET);

    LexemeSequence sequence5(&text, 8, 4, 1);
    QCOMPARE(sequence5.state(), LexemeSequence::STATE_BAD_OFFSET_N);

    LexemeSequence sequence6(&text, 8, 2, 1);
    QCOMPARE(sequence6.state(), LexemeSequence::STATE_HAS_BOUNDARIES);
}

void TestLexemeSequence::simpleSequence()
{
    const char *_text =
       "A database connection string is a special format string "
       "that is passed to the database driver each time a "
       "database connection is performed. It is very important to "
       "specify correct setting in the database connection string "
       "since default connection parameters will generally not work."
    ;
    Text text;
    text.append(QString(_text));

    /* Extract a 3-gram: (datbase connection, string) */
    LexemeSequence sequence(&text, 1, 3, 2);
    QCOMPARE(sequence.state(), LexemeSequence::STATE_OK);
    QCOMPARE(sequence.isValid(), true);
    QCOMPARE(sequence.length(), (ulong)3);
    QCOMPARE(sequence.boundary(), (ulong)2);
    QCOMPARE(sequence.lexemes()->length(), 3);
    QCOMPARE(sequence.sequenceKey()->length(), (int)(3 * sizeof(ulong)));
    QCOMPARE(sequence.mi(), 46 * (double)(2.0 / (3.0 * 3.0)));
//    QCOMPARE(sequence.llr(), 0.0);
    QCOMPARE(sequence.score(), sequence.llr());
}

QTEST_MAIN(TestLexemeSequence)
#include "test_lexeme_sequence.moc"
