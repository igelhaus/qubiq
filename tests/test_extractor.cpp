#include <QtTest/QtTest>

#include <qubiq/extractor.h>

const char *_text =
   "A database connection string is a special format string "     // 00 .. 08
   "that is passed to the database driver each time a "           // 09 .. 18
   "database connection is performed. It is very important to "   // 19 .. 28
   "specify correct setting in the database connection string "   // 29 .. 36
   "since default connection parameters will generally not work." // 37 .. 45
;

class TestExtractor: public QObject
{
    Q_OBJECT

private slots:
    void emptyExtractor();
    void simpleExtractor();
};

void TestExtractor::emptyExtractor()
{
    Text text;
    text.append(QString(_text));

    Extractor extractor(&text);
    QCOMPARE(extractor.minBigramFrequency(),        DEFAULT_MIN_BIGRAM_FREQUENCY);
    QCOMPARE(extractor.maxSourceExtractionRate(),   DEFAULT_MAX_SOURCE_EXTRACTION_RATE);
    QCOMPARE(extractor.maxLeftExpansionDistance(),  DEFAULT_MAX_LEFT_EXPANSION_DISTANCE);
    QCOMPARE(extractor.maxRightExpansionDistance(), DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE);
    QCOMPARE(extractor.qualityDecreaseThreshold(),  DEFAULT_QUALITY_DECREASE_THRESHOLD);
}

void TestExtractor::simpleExtractor()
{
    Text text;
    text.append(QString(_text));

    Extractor extractor(&text);
    const QList<LexemeSequence> *extracted;

    QCOMPARE(extractor.extract(), true);
    extracted = extractor.extracted();
    for (int i = 0; i < extracted->size(); i++) {
        const LexemeSequence &term = extracted->at(i);
        qDebug() << term.image(extractor.text()) << " " << term.mi() << " " << term.llr();
    }
    extractor.setMaxLeftExpansionDistance(0);
    QCOMPARE(extractor.extract(), true);
    extracted = extractor.extracted();
    for (int i = 0; i < extracted->size(); i++) {
        const LexemeSequence &term = extracted->at(i);
        qDebug() << term.image(extractor.text()) << " " << term.mi() << " " << term.llr();
    }

    extractor.setMaxSourceExtractionRate(0.5);
    QCOMPARE(extractor.extract(), true);
    extracted = extractor.extracted();
    for (int i = 0; i < extracted->size(); i++) {
        const LexemeSequence &term = extracted->at(i);
        qDebug() << term.image(extractor.text()) << " " << term.mi() << " " << term.llr();
    }
}

QTEST_MAIN(TestExtractor)
#include "test_extractor.moc"
