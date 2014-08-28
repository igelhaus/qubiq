#ifndef _EXTRACTOR_H_
#define _EXTRACTOR_H_

#include <algorithm>
#include <QtCore>
#include <cutelogger/include/Logger.h>
#include <qubiq/global.h>
#include <qubiq/lexeme.h>
#include <qubiq/text.h>
#include <qubiq/lexeme_sequence.h>
#include <qubiq/abstract_term_filter.h>

const int    DEFAULT_MIN_BIGRAM_FREQUENCY         = 3;   //!< Default minimum bigram frequency
const double DEFAULT_MIN_BIGRAM_SCORE             = 5.0; //!< Default minimum bigram score
const double DEFAULT_MAX_SOURCE_EXTRACTION_RATE   = 0.3; //!< Default maximum source extraction rate
const int    DEFAULT_MAX_LEFT_EXPANSION_DISTANCE  = 2;   //!< Default maximum left expansion distance
const int    DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE = 2;   //!< Default maximum right expansion distance
const double DEFAULT_QUALITY_DECREASE_THRESHOLD   = 3.0; //!< Default quality decrease threshold

class QUBIQSHARED_EXPORT Extractor : public QObject {
    Q_OBJECT

public:
    Extractor(const Text *text);
    ~Extractor();

    bool extract(bool sort_terms = false);

    //! Returns post-extraction filter currently associated with the extractor.
    inline AbstractTermFilter* filter() const { return _filter; }

    //! Associates a new post-extraction filter with the extractor.
    inline void setFilter(AbstractTermFilter *filter) { _filter = filter; }

    /**
     * Returns minimum bigram frequency.
     *
     * Only bigrams with frequency higher or equal to this value will
     * be extracted as term candidates.
     *
     * \sa minScore
     * \sa setMinBigramFrequency
     */
    inline int minBigramFrequency() const { return _min_bf; }
    //! Sets minimum bigram frequency
    inline void setMinBigramFrequency(int bf) { _min_bf = bf; }

    /**
     * Returns minimum bigram score.
     *
     * Only bigrams with overall score higher or equal to this value will
     * be extracted as term candidates.
     *
     * \sa minBigramFrequency
     * \sa setMinScore
     */
    inline double minBigramScore() const { return _min_bs; }
    //! Sets minimum bigram score
    inline void setMinBigramScore(double score) { _min_bs = score; }

    /**
     * Returns maximum source extraction rate.
     *
     * If a sequence produces good expansions, we need to decide whether the
     * sequence is a term on its own or whether it is just a part of good terms.
     * Sequences with `(number_of_expansions / frequency)` ratio lower or equal
     * to this value will be treated as terms, otherwise the sequence will not be
     * extracted.
     *
     * \sa setMaxSourceExtractionRate
     */
    inline double maxSourceExtractionRate() const { return _max_ser; }
    //! Sets maximum source extraction rate.
    inline void setMaxSourceExtractionRate(double max_ser){ _max_ser = max_ser; }

    /**
     * Returns maximum left expansion distance.
     *
     * This parameter sets maximum number of lexemes that can be attached to
     * the left of the source bigram while trying to expand it to a better term.
     *
     * \sa maxRightExpansionDistance
     * \sa setMaxLeftExpansionDistance
     */
    inline int maxLeftExpansionDistance() const { return _max_led; }
    //! Sets maximum left expansion distance.
    inline void setMaxLeftExpansionDistance (int max_led) { _max_led = max_led; }

    /**
     * Returns maximum right expansion distance.
     *
     * This parameter sets maximum number of lexemes that can be attached to
     * the right of the source bigram while trying to expand it to a better term.
     *
     * \sa maxLeftExpansionDistance
     * \sa setMaxRightExpansionDistance
     */
    inline int maxRightExpansionDistance() const { return _max_red; }
    //! Sets maximum right expansion distance.
    inline void setMaxRightExpansionDistance(int max_red) { _max_red = max_red; }

    /**
     * Returns quality decrease threshold.
     *
     * This parameter is used to measure quality of expanded sequences.
     * Expanded lexeme sequences are extracted only if
     * `score(expanded) >= score(source) - qdt`.
     *
     * \sa setQualityDecreaseThreshold
     */
    inline double qualityDecreaseThreshold() const { return _qdt; }
    //! Sets quality decrease threshold.
    inline void setQualityDecreaseThreshold(double qdt) { _qdt = qdt; }

    //! Returns pointer to the source text.
    inline const Text *text() const { return _text; }

    //! Returns pointer to the list of extracted terms.
    inline const QList<LexemeSequence> *extracted() const { return _candidates; }

private:
    const Text *_text;
    AbstractTermFilter *_filter;

    int     _min_bf; //!< Minimum bigram frequency
    double _min_bs;  //!< Minimum bigram score
    double _max_ser; //!< Maximum source extraction rate
    int    _max_led; //!< Maximum left expansion distance
    int    _max_red; //!< Maximum right expansion distance
    double     _qdt; //!< Quality decrease threshold

    QList<LexemeSequence> *_candidates; //!< List of term candidates and (later) final terms
    QSet<QByteArray>      *_extracted;  //!< Set used to ensure that candidates are extracted only once

    void _initialize();
    void _destroy();

    bool collect_good_bigrams();
    bool is_good_bigram   (const LexemeSequence &bigram) const;
    bool treat_as_term    (const LexemeSequence &bigram, int num_expansions) const;
    int  expand           (const LexemeSequence &candidate, bool is_left_expanded);
    bool validate_expanded(const LexemeSequence &expanded, const LexemeSequence &source) const;
    bool has_better_score (const LexemeSequence &expanded, const LexemeSequence &source) const;
    int  store_expanded   (LexemeSequence *expanded, const LexemeSequence &source, bool is_left_expanded);
};

#endif // _EXTRACTOR_H_
