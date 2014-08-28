#include <QtCore>
#include <cutelogger/include/Logger.h>
#include <cutelogger/include/FileAppender.h>
#include <qubiq/extractor.h>
#include <qubiq/abstract_term_filter.h>

#include <iostream>

class EnglishTermFilter: public AbstractTermFilter {
public:
    EnglishTermFilter();
    virtual ~EnglishTermFilter();

    virtual bool passes(const LexemeSequence &sequence);

private:
    QSet<QString> *_articles;
    QSet<QString> *_conjunctions;
    QSet<QString> *_prepositions;
    QSet<QString> *_demonstratives;
};

EnglishTermFilter::EnglishTermFilter()
{
    _articles       = new QSet<QString>();
    _conjunctions   = new QSet<QString>();
    _prepositions   = new QSet<QString>();
    _demonstratives = new QSet<QString>();

    (*_articles) << "a" << "an" << "the";

    (*_conjunctions) << "and" << "or";

    (*_prepositions)
            << "abaft"
            << "abeam"
            << "aboard"
            << "about"
            << "above"
            << "absent"
            << "across"
            << "afore"
            << "after"
            << "against"
            << "along"
            << "alongside"
            << "amid"
            << "amidst"
            << "among"
            << "amongst"
            << "anenst"
            << "apropos"
            << "apud"
            << "around"
            << "as"
            << "aside"
            << "astride"
            << "at"
            << "athwart"
            << "atop"
            << "barring"
            << "before"
            << "behind"
            << "below"
            << "beneath"
            << "beside"
            << "besides"
            << "between"
            << "beyond"
            << "but"
            << "by"
            << "circa"
            << "concerning"
            << "despite"
            << "down"
            << "during"
            << "except"
            << "excluding"
            << "failing"
            << "following"
            << "for"
            << "forenenst"
            << "from"
            << "given"
            << "in"
            << "including"
            << "inside"
            << "into"
            << "like"
            << "mid"
            << "midst"
            << "minus"
            << "modulo"
            << "near"
            << "next"
            << "notwithstanding"
            << "of"
            << "off"
            << "on"
            << "onto"
            << "opposite"
            << "out"
            << "outside"
            << "over"
            << "pace"
            << "past"
            << "per"
            << "plus"
            << "pro"
            << "qua"
            << "regarding"
            << "round"
            << "sans"
            << "save"
            << "since"
            << "than"
            << "through"
            << "throughout"
            << "till"
            << "times"
            << "to"
            << "toward"
            << "towards"
            << "under"
            << "underneath"
            << "unlike"
            << "until"
            << "unto"
            << "up"
            << "upon"
            << "versus"
            << "via"
            << "vice"
            << "with"
            << "within"
            << "without"
            << "worth"
    ;

    (*_demonstratives) << "this" << "these" << "that" << "those";
}

EnglishTermFilter::~EnglishTermFilter()
{
    delete _articles;
    delete _conjunctions;
    delete _prepositions;
    delete _demonstratives;
}

bool EnglishTermFilter::passes(const LexemeSequence &sequence)
{
    const Text *text = sequence.text();

    int idx_first_lexeme = sequence.lexemes()->at(0);
    int idx_last_lexeme  = sequence.lexemes()->at(sequence.lexemes()->size() - 1);
    QString first_lexeme = text->lexemes()->at(idx_first_lexeme)->lexeme();
    QString last_lexeme  = text->lexemes()->at(idx_last_lexeme)->lexeme();

    if (_prepositions->contains(first_lexeme) || _prepositions->contains(last_lexeme)) {
//        LOG_DEBUG() << sequence.image() << " rejected: starts or ends with a preposition";
        return false;
    }

    if (_conjunctions->contains(first_lexeme) || _conjunctions->contains(last_lexeme)) {
        return false;
    }

    if (_articles->contains(first_lexeme) || _articles->contains(last_lexeme)) {
        return false;
    }

    if (_demonstratives->contains(first_lexeme) || _demonstratives->contains(last_lexeme)) {
        return false;
    }

    return true;
}

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
    ), optLanguage(QStringList() << "l" << "language",
        "[STRING] Lowercase, two-letter, ISO 639 language code of the input."
        " If omitted, English (or, more precisely, \"C\" locale) is assumed.",
        "language"
    ),
    optFiles("file",
        "[STRING, MULTIPLE] Path to file(s) to extract terms from."
        " If omitted, text will be read from stdin.",
        "file"
    ), optMinBigramFrequency("mbf",
        "[INTEGER] Minimum bigram frequency: Only bigrams with frequency higher"
        " or equal to this value will be extracted as term candidates."
        " Used together with --mbs.",
        "mbf"
    ), optMinBigramScore("mbs",
        "[DOUBLE] Minimum bigram core: Only bigrams with score higher"
        " or equal to this value will be extracted as term candidates."
        " Used together with --mbf.",
        "mbs"
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
    parser.addOption(optLanguage);
    parser.addOption(optFiles);
    parser.addOption(optMinBigramFrequency);
    parser.addOption(optMinBigramScore);
    parser.addOption(optMaxSourceExtractionRate);
    parser.addOption(optMaxLeftExpansionDistance);
    parser.addOption(optMaxRightExpansionDistance);
    parser.addOption(optQualityDecreaseThreshold);

    parser.process(app);

    FileAppender *log_file = new FileAppender("extract-terms.log");
    log_file->setDetailsLevel(parser.value(optLogLevel));
    logger->registerAppender(log_file);

    QString language = parser.value(optLanguage);
    QLocale locale(language);

    Text text(locale);

    const QStringList files = parser.values(optFiles);
    if (files.size() == 0) {
        text.appendFile(stdin);
    } else {
        for (int i = 0; i < files.size(); i++) {
            text.appendFile(files.at(i));
        }
    }

    Extractor extractor(&text);
    EnglishTermFilter english_filter;

    if (language.left(2).toLower() == "en")
        extractor.setFilter(&english_filter);

    bool is_converted = true;

    int mbf = parser.value(optMinBigramFrequency).toInt(&is_converted);
    if (is_converted)
        extractor.setMinBigramFrequency(mbf);

    double mbs = parser.value(optMinBigramScore).toDouble(&is_converted);
    if (is_converted)
        extractor.setMinBigramScore(mbs);

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

    bool extracted = extractor.extract(true);

    if (extracted) {
        const QList<LexemeSequence> *terms = extractor.extracted();
        for (int i = 0; i < terms->size(); i++) {
            std::cout
                << terms->at(i).image().toUtf8().data()
                << "\t"
                << terms->at(i).score()
                << std::endl
            ;
        }
    }

    return 1;
}
