#include <QtCore>
#include <cutelogger/include/Logger.h>
#include <cutelogger/include/FileAppender.h>
#include <qubiq/extractor.h>

#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("extract-terms");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "extract-terms: Simple command-line utility"
        " for extracting terms using libqubiq"
    );
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption optLogLevel("log-level",
        "[STRING] Level of logging. Recognized values are:"
        " trace, debug, info (default), warning, error, fatal",
        "log-level",
        "info"
    ), optFiles("file",
        "[STRING, MULTIPLE] Path to file(s) to extract terms from.",
        "file"
    ), optMinBigramFrequency("mbf",
        "[INTEGER] Minimum bigram frequency: Only bigrams with frequency higher"
        " or equal to this value will be extracted as term candidates.",
        "mbf"
    ), optMaxSourceExtractionRate("mser",
        "[DOUBLE] Maximum source extraction rate: Only sequences with (number_of_expansions / frequency)"
        " ratio lower or equal to this value will be treated as terms themselves"
        " in case they generate expansions during term extractions.",
        "mser"
    ), optMaxLeftExpansionDistance("mled",
        "[INTEGER] Maximum left expansion distance: Maximum length of sequence adjacent"
        " to the left of each bigram term candidate, and used for expanding"
        " initially extracted bigrams into multiword terms.",
        "mled"
    ), optMaxRightExpansionDistance("mred",
        "[INTEGER] Maximum right expansion distance: Maximum length of sequence adjacent"
        " to the right of each bigram term candidate, and used for expanding"
        " initially extracted bigrams into multiword terms.",
        "mred"
    ), optQualityDecreaseThreshold("qdt",
        "[DOUBLE] Quality decrease threshold: Expanded lexeme sequences are extracted"
        " only if score(expanded) >= score(source) - qdt.",
        "qdt"
    );
    parser.addOption(optLogLevel);
    parser.addOption(optFiles);
    parser.addOption(optMinBigramFrequency);
    parser.addOption(optMaxSourceExtractionRate);
    parser.addOption(optMaxLeftExpansionDistance);
    parser.addOption(optMaxRightExpansionDistance);
    parser.addOption(optQualityDecreaseThreshold);

    parser.process(app);

    FileAppender *log_file = new FileAppender("extract-terms.log");
    log_file->setDetailsLevel(parser.value(optLogLevel));
    logger->registerAppender(log_file);

    // FIXME: read from stdin if no files specified
    Text text;
    const QStringList files = parser.values(optFiles);
    for (int i = 0; i < files.size(); i++) {
        text.appendFile(files.at(i));
    }

    Extractor extractor(&text);

    bool is_converted = true;

    int mbf = parser.value(optMinBigramFrequency).toInt(&is_converted);
    if (is_converted)
        extractor.setMinBigramFrequency(mbf);

    double mser = parser.value(optMaxSourceExtractionRate).toDouble(&is_converted);
    if (is_converted)
        extractor.setMaxSourceExtractionRate(mser);

    int mled = parser.value(optMaxLeftExpansionDistance).toInt(&is_converted);
    if (is_converted)
        extractor.setMaxLeftExpansionDistance(mled);

    int mred = parser.value(optMaxRightExpansionDistance).toInt(&is_converted);
    if (is_converted)
        extractor.setMaxRightExpansionDistance(mred);

    double qdt = parser.value(optQualityDecreaseThreshold).toDouble(&is_converted);
    if (is_converted)
        extractor.setQualityDecreaseThreshold(qdt);

    bool extracted = extractor.extract();

    if (extracted) {
        const QList<LexemeSequence> *terms = extractor.extracted();
        for (int i = 0; i < terms->size(); i++) {
            std::cout
                << terms->at(i).image(&text).toUtf8().data()
                << "\t"
                << terms->at(i).score()
                << std::endl
            ;
        }
    }

    return 1;
}
