#include <QtTest/QtTest>
#include <cmath>
#include <qubiq/lexeme_sequence.h>

const char *_text =
   "A database connection string is a special format string "     // 00 .. 08
   "that is passed to the database driver each time a "           // 09 .. 18
   "database connection is performed. It is very important to "   // 19 .. 28
   "specify correct setting in the database connection string "   // 29 .. 36
   "since default connection parameters will generally not work." // 37 .. 45
;

class TestLexemeSequence: public QObject
{
    Q_OBJECT

private slots:
    void emptySequence();
    void emptySequenceCopy();
    void emptySequenceAssignment();
    void badSequenceStates();
    void simpleSequence();
    void extremeMetricValues();
    void comparisonOperator();
    void hashOfSequences();
    void setOfSequences();
};

void TestLexemeSequence::emptySequence()
{
    LexemeSequence sequence;
    QCOMPARE(sequence.state(), LexemeSequence::STATE_EMPTY);
    QCOMPARE(sequence.isValid(), false);
    QCOMPARE(sequence.length(), 0);
    QCOMPARE(sequence.n1(), 0);
    QCOMPARE(sequence.lexemes()->length(), 0);
    QCOMPARE(sequence.key()->length(), 0);
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
    QCOMPARE(sequence2.length(), 0);
    QCOMPARE(sequence2.n1(), 0);
    QCOMPARE(sequence2.lexemes()->length(), 0);
    QCOMPARE(sequence2.key()->length(), 0);
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
    QCOMPARE(sequence2.length(), 0);
    QCOMPARE(sequence2.n1(), 0);
    QCOMPARE(sequence2.lexemes()->length(), 0);
    QCOMPARE(sequence2.key()->length(), 0);
    QCOMPARE(sequence2.mi(), 0.0);
    QCOMPARE(sequence2.llr(), 0.0);
    QCOMPARE(sequence2.score(), 0.0);
}

void TestLexemeSequence::badSequenceStates()
{
    Text text;

    text.append(QString("The quick brown fox jumps over the lazy dog."));

    LexemeSequence sequence1(NULL, 1, 2, 1);
    QCOMPARE(sequence1.state(), LexemeSequence::STATE_BAD_TEXT);

    LexemeSequence sequence2(&text, 1, 1, 1);
    QCOMPARE(sequence2.state(), LexemeSequence::STATE_UNIGRAM);

    LexemeSequence sequence3(&text, 1, 2, 0);
    QCOMPARE(sequence3.state(), LexemeSequence::STATE_BAD_BOUNDARY);

    LexemeSequence sequence4(&text, 1, 2, 2);
    QCOMPARE(sequence4.state(), LexemeSequence::STATE_BAD_BOUNDARY);

    LexemeSequence sequence5(&text, 10, 2, 1);
    QCOMPARE(sequence5.state(), LexemeSequence::STATE_BAD_OFFSET);

    LexemeSequence sequence6(&text, 8, 4, 1);
    QCOMPARE(sequence6.state(), LexemeSequence::STATE_BAD_OFFSET_N);

    LexemeSequence sequence7(&text, 8, 2, 1);
    QCOMPARE(sequence7.state(), LexemeSequence::STATE_HAS_BOUNDARIES);
}

void TestLexemeSequence::simpleSequence()
{
    Text text;
    text.append(QString(_text));

    /* Extract a trigram: (datbase connection, string) */
    LexemeSequence sequence(&text, 1, 3, 2);
    QCOMPARE(sequence.state(), LexemeSequence::STATE_OK);
    QCOMPARE(sequence.isValid(), true);
    QCOMPARE(sequence.length(), 3);
    QCOMPARE(sequence.n1(), 2);
    QCOMPARE(sequence.lexemes()->length(), 3);
    QCOMPARE(sequence.key()->length(), (int)(3 * sizeof(quintptr)));
    QCOMPARE(sequence.frequency(), 2);
    QCOMPARE(sequence.mi(), 46 * (double)(2.0 / (3.0 * 3.0)));
    QCOMPARE(sequence.llr() > 0.0, true);
    QCOMPARE(sequence.score(), sequence.llr());
}

void TestLexemeSequence::extremeMetricValues()
{
    // 1. Artificial text
    Text text1;
    text1.append(QString("x x"));
    LexemeSequence sequence1(&text1, 0, 2, 1);
    QCOMPARE(sequence1.isValid(), true);
    QCOMPARE(!std::isnan(sequence1.score()), true);
    QCOMPARE(sequence1.score() == 0.0, true);

    // 2. The first subsequence always belongs to the sequence
    Text text2;
    text2.append(QString(
        "first lexeme of the sequence in this text is the lexeme"
        " that belongs to the first lexeme sequence only"
    ));
    LexemeSequence sequence2(&text2, 0, 2, 1);
    QCOMPARE(sequence2.isValid(), true);
    QCOMPARE(!std::isnan(sequence2.score()), true);
    QCOMPARE(sequence2.score() > 0.0, true);

    // 3. The second subsequence always belongs to the sequence
    Text text3;
    text3.append(QString(
        "second lexeme of the sequence belongs to the"
        " second lexeme sequence only: second adjacency test"
    ));
    LexemeSequence sequence3(&text3, 0, 2, 1);
    QCOMPARE(sequence3.isValid(), true);
    QCOMPARE(!std::isnan(sequence3.score()), true);
    QCOMPARE(sequence3.score() > 0.0, true);

    // 4. Subsequences are always adjacent to each other
    Text text4;
    text4.append(QString(
        "lexeme sequence lexeme sequence in this text consists of"
        " subsequences that are adjacent to each other in the lexeme sequence"
    ));
    LexemeSequence sequence4(&text4, 0, 2, 1);
    QCOMPARE(sequence4.isValid(), true);
    QCOMPARE(!std::isnan(sequence4.score()), true);
    QCOMPARE(sequence4.score() > 0.0, true);
}

void TestLexemeSequence::comparisonOperator()
{
    Text text;
    text.append(QString(_text));

    /* Extract a trigram: (datbase connection, string) */
    LexemeSequence sequence1(&text, 1, 3, 2);
    /* Extract the same trigram, but from another offset */
    LexemeSequence sequence2(&text, 34, 3, 2);
    /* Extract a different trigram: (connection string, is) */
    LexemeSequence sequence3(&text, 2, 3, 2);

    QCOMPARE(sequence1 == sequence2, true);
    QCOMPARE(sequence1 == sequence3, false);

    QCOMPARE(*(sequence1.key()) == *(sequence2.key()), true);
    QCOMPARE(*(sequence1.key()) == *(sequence3.key()), false);
    QCOMPARE(qHash(sequence1, 0) == qHash(sequence2, 0), true);
    QCOMPARE(qHash(sequence1, 0) == qHash(sequence3, 0), false);
}

void TestLexemeSequence::hashOfSequences()
{
    Text text;
    text.append(QString(_text));

    /* Extract a trigram: (datbase connection, string) */
    LexemeSequence sequence1(&text, 1, 3, 2);
    /* Extract the same trigram, but from another offset */
    LexemeSequence sequence2(&text, 34, 3, 2);
    /* Extract a different trigram: (connection string, is) */
    LexemeSequence sequence3(&text, 2, 3, 2);

    QHash<LexemeSequence, bool> sequences;

    sequences.insert(sequence1, true);
    QCOMPARE(sequences.keys().length(), 1);
    QCOMPARE(sequences.contains(sequence1), true);
    QCOMPARE(sequences.contains(sequence2), true);
    QCOMPARE(sequences.contains(sequence3), false);

    sequences.insert(sequence3, true);
    QCOMPARE(sequences.keys().length(), 2);
}

void TestLexemeSequence::setOfSequences()
{
    Text text;
    text.append(QString(_text));

    /* Extract a trigram: (datbase connection, string) */
    LexemeSequence sequence1(&text, 1, 3, 2);
    /* Extract the same trigram, but from another offset */
    LexemeSequence sequence2(&text, 34, 3, 2);
    /* Extract a different trigram: (connection string, is) */
    LexemeSequence sequence3(&text, 2, 3, 2);

    QSet<LexemeSequence> sequences;

    sequences.insert(sequence1);
    QCOMPARE(sequences.size(), 1);
    QCOMPARE(sequences.contains(sequence1), true);
    QCOMPARE(sequences.contains(sequence2), true);
    QCOMPARE(sequences.contains(sequence3), false);

    sequences.insert(sequence3);
    QCOMPARE(sequences.size(), 2);
}

QTEST_MAIN(TestLexemeSequence)
#include "test_lexeme_sequence.moc"
