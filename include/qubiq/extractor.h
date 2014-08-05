#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include <QtCore>
#include <qubiq/lexeme.h>
#include <qubiq/text.h>
#include <qubiq/lexeme_sequence.h>

const int    DEFAULT_MIN_BIGRAM_FREQUENCY         = 3;
const double DEFAULT_BIGRAM_EXTRACTION_RATE       = 0.3; // FIXME: adjust me
const int    DEFAULT_MAX_LEFT_EXPANSION_DISTANCE  = 2;
const int    DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE = 2;
const double DEFAULT_QUALITY_DECREASE_THRESHOLD   = 3.0; // FIXME: adjust me

class Extractor : public QObject {
    Q_OBJECT

public:
    Extractor(const Text *text);
    ~Extractor();

    bool extract();

    inline int    minBigramFrequency()        const { return _min_bf; }
    inline double bigramExtractionRate()      const { return _ber; }
    inline int    maxLeftExpansionDistance()  const { return _max_led; }
    inline int    maxRightExpansionDistance() const { return _max_red; }
    inline double qualityDecreaseThreshold()  const { return _qdt; }

    inline void setMinBigramFrequency(int bf)             { _min_bf = bf; }
    inline void setBigramExtractionRate(double ber)       { _ber = ber; }
    inline void setMaxLeftExpansionDistance (int max_led) { _max_led = max_led; }
    inline void setMaxRightExpansionDistance(int max_red) { _max_red = max_red; }
    inline void setQualityDecreaseThreshold(double qdt)   { _qdt = qdt; }

    inline const Text *text() const { return _text; }
    inline const QList<LexemeSequence> *extracted() const { return _candidates; }

private:
    const Text *_text;
    int    _min_bf; /* Minimum bigram frequency */
    double    _ber; /* Bigram extraction rate */
    int   _max_led; /* Maximum left expansion distance */
    int   _max_red; /* Maximum right expansion distance */
    double    _qdt; /* Quality decrease threshold */

    QList<LexemeSequence> *_candidates;
    QSet<QByteArray>      *_extracted;

    bool collect_good_bigrams();
    bool is_good_bigram   (const LexemeSequence &bigram) const;
    bool treat_as_term    (const LexemeSequence &bigram, int num_expansions) const;
    int  expand           (const LexemeSequence &candidate, bool is_left_expanded);
    bool validate_expanded(const LexemeSequence &expanded, const LexemeSequence &source) const;
    bool has_better_score (const LexemeSequence &expanded, const LexemeSequence &source) const;
    int  store_expanded   (LexemeSequence *expanded, const LexemeSequence &source, bool is_left_expanded);
};

#endif // _EXTRACTOR_H_
