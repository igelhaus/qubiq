#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include <QtCore>
#include <qubiq/lexeme.h>
#include <qubiq/text.h>
#include <qubiq/lexeme_sequence.h>

const ulong  DEFAULT_MIN_BIGRAM_FREQUENCY         = 3;
const double DEFAULT_BIGRAM_EXTRACTION_RATE       = 0.3; // FIXME: adjust me
const ulong  DEFAULT_MAX_LEFT_EXPANSION_DISTANCE  = 2;
const ulong  DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE = 2;
const double DEFAULT_QUALITY_DECREASE_THRESHOLD   = 3.0; // FIXME: adjust me

class Extractor : public QObject {
    Q_OBJECT

public:
    Extractor(const Text *text);
    ~Extractor();

    bool extract();

    inline ulong  minBigramFrequency()        const { return _min_bf; }
    inline double bigramExtractionrate()      const { return _ber; }
    inline ulong  maxLeftExpansionDistance()  const { return _max_led; }
    inline ulong  maxRightExpansionDistance() const { return _max_red; }
    inline double qualityDecreaseThreshold()  const { return _qdt; }

    inline void setMinBigramFrequency(ulong bf)             { _min_bf = bf; }
    inline void setBigramExtractionRate(double ber)         { _ber = ber; }
    inline void setMaxLeftExpansionDistance (ulong max_led) { _max_led = max_led; }
    inline void setMaxRightExpansionDistance(ulong max_red) { _max_red = max_red; }
    inline void setQualityDecreaseThreshold(double qdt)     { _qdt = qdt; }

private:
    Text *_text;
    ulong _min_bf;  /* Minimum bigram frequency */
    double _ber;    /* Bigram extraction rate */
    ulong _max_led; /* Maximum left expansion distance */
    ulong _max_red; /* Maximum right expansion distance */
    double _qdt;    /* Quality decrease threshold */

    QList<LexemeSequence> *_candidates;
    QSet<QByteArray>      *_extracted;

    bool collect_good_bigrams();
    bool is_good_bigram(const LexemeSequence &bigram);
    bool treat_bigram_as_term(int i, int num_expansions);
    int  expand_left(int i);
    int  expand_right(int i);
    bool has_better_score(const LexemeSequence &expanded, const LexemeSequence &source);
};

#endif // _EXTRACTOR_H_
