#include <qubiq/extractor.h>

/**
 * \brief Constructs an Extractor object.
 * \param[in] text Text to extract terms from.
 */
Extractor::Extractor(const Text *text)
{
    _text      = text;
    _min_bf    = DEFAULT_MIN_BIGRAM_FREQUENCY;
    _min_score = DEFAULT_MIN_SCORE;
    _max_ser   = DEFAULT_MAX_SOURCE_EXTRACTION_RATE;
    _max_led   = DEFAULT_MAX_LEFT_EXPANSION_DISTANCE;
    _max_red   = DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE;
    _qdt       = DEFAULT_QUALITY_DECREASE_THRESHOLD;

    _initialize();
}

//! Destructs the Extrator object.
Extractor::~Extractor()
{
    _destroy();
}

/**
 * \brief Extracts terms from the text.
 *
 * This is the main routine for term extraction. Without some details,
 * the process of extraction is done in the following way:
 *
 * -# Method intializes internal buffers for storing intermediate and final results.
 * -# Then all good bigrams are collected into a list of term candidates. A bigram
 *    is good if it is a valid sequence of two lexemes with frequency more or
 *    equal to \c minBigramFrequency and over score more or equal to \c minScore.
 *    Each good bigram is extracted only once.
 * -# Method attempts to expand each good bigram to check whether expanded sequences
 *    for a good term. Expansion is done by appending all possible left and right-adjacent
 *    lexemes to the bigram and measuring the scores of the resulting sequences.
 * -# All good expansions are added to the list of term candidates.
 * -# Method continues processing the list of candidates trying to expand new candidates.
 *    Expansions are limited with \c maxLeftExpansionDistance and
 *    \c maxRightExpansionDistance values, so at a certain step new expansions
 *    will not be generated and all candidates will be processed.
 * -# Sequences that reside in the candidate list are extracted terms.
 *
 * \returns \c true if at least one term was extracted and \false otherwise.
 */
bool Extractor::extract()
{
    LOG_INFO("Starting extraction");
    LOG_INFO() << "min_bf    =" << _min_bf;
    LOG_INFO() << "min_score =" << _min_score;
    LOG_INFO() << "max_ser   =" << _max_ser;
    LOG_INFO() << "max_led   =" << _max_led;
    LOG_INFO() << "max_red   =" << _max_red;
    LOG_INFO() << "qdt       =" << _qdt;

    _destroy();
    _initialize();

    if (!collect_good_bigrams()) {
        LOG_WARNING("No good bigrams collected, consider adjusting minBigramFrequency");
        return false;
    }

    LOG_INFO("Starting expandindg good bigrams");

    int i = 0;
    while (i < _candidates->size()) {
        const LexemeSequence &candidate = _candidates->at(i);
        int num_expanded_left  = 0;
        int num_expanded_right = 0;
        int num_expansions     = 0;

        if (candidate.leftExpansionDistance() < _max_led)
            num_expanded_left = expand(candidate, true);

        if (candidate.rightExpansionDistance() < _max_red)
            num_expanded_right = expand(candidate, false);

        num_expansions = num_expanded_left + num_expanded_right;
        LOG_DEBUG() << "Expanding candidate:" << candidate.image(_text);
        LOG_DEBUG() << "Expansions made (left, right, total):"
                    << num_expanded_left
                    << num_expanded_right
                    << num_expansions;

        // No expansions: Leave current candidate as a term and go to next
        // At least one expansion: Decide whether we can leave current candidate as a term
        if (num_expansions == 0 || treat_as_term(candidate, num_expansions)) {
            i++;
            continue;
        }
        // Current candidate is not a good term, remove it from the list:
        _extracted->remove(*(candidate.key()));
        _candidates->removeAt(i);
    }

    LOG_INFO("Extraction finished");

    // FIXME: make optional
    if (_candidates->size() > 0)
        std::sort(_candidates->begin(), _candidates->end(), hasBetterSequence);


    return true;
}

/**
 * \brief Collects good bigrams into the list of term candidates for further expansion.
 * \returns \c true if at least one bigram was extracted and \c false otherwise.
 * \sa minBigramFrequency
 * \sa setMinBigramFrequency
 * \sa minScore
 * \sa setMinScore
 * \sa is_good_bigram
 */
bool Extractor::collect_good_bigrams()
{
    LOG_INFO("Starting collecting good bigrams");
    for (int i = 0; i < _text->length(); i++) {
        LexemeSequence bigram(_text, i, 2, 1);
        if (!bigram.isValid())
            continue;
        const QByteArray *key = bigram.key();
        if (!_extracted->contains(*key) && is_good_bigram(bigram)) {
            LOG_DEBUG()
                << "Found at offset" << i << ": "
                << bigram.image(_text) << "\t" << bigram.score();
            _extracted->insert(*key);
            _candidates->append(bigram);
        }
    }
    LOG_INFO() << _extracted->size() << "good bigrams collected";
    return _extracted->size() > 0;
}

/**
 * \brief Evaluates quality of a bigram.
 * \param[in] bigram Bigram to be evaluated.
 * \returns \c true if the bigram is considered good and \c false otherwise.
 * \sa minBigramFrequency
 * \sa setMinBigramFrequency
 * \sa minScore
 * \sa setMinScore
 * \sa collect_good_bigrams
 */
bool Extractor::is_good_bigram(const LexemeSequence &bigram) const
{
    return bigram.frequency() >= _min_bf && bigram.score() >= _min_score;
}

/**
 * \brief Evaluates whether a sequence (of any length) should be treated as a term.
 *
 * This method calculates a ratio of number of expansions produced by the
 * evaluated sequence to frequency of the sequence in the source text. If this
 * ratio is less or equal to \c maxSourceExtractionRate then the sequence is
 * treated as a term. Rationale: If most occurrences of a candidate in the text
 * have not been expanded then it is probably a term.
 *
 * \param[in] candidate      Candidate sequence to evaluate.
 * \param[in] num_expansions Number of expansions produced by the candidate.
 * \returns \c true if the candidate is a term and \c false otherwise.
 */
bool Extractor::treat_as_term(const LexemeSequence &candidate, int num_expansions) const
{
    double exp_ratio = (double)num_expansions / (double)candidate.frequency();
    LOG_DEBUG() << "Expansion ratio:" << exp_ratio;
    return exp_ratio <= _max_ser;
}

/**
 * \brief Expands a sequence (of any length) by one lexeme left or right.
 * \param[in] candidate        Sequence to be expanded (aka source sequence).
 * \param[in] is_left_expanded Expansion direction: left (\c true) or right (\c false).
 * \returns Number of produced expansions.
 */
int Extractor::expand(const LexemeSequence &candidate, bool is_left_expanded)
{
    int num_expanded = 0;
    const QVector<int> *first_lexeme_entries = candidate.offsets();
    int n  = candidate.length() + 1;
    int n1 = is_left_expanded? 1 : candidate.length();

    for (int i = 0; i < first_lexeme_entries->size(); i++) {
        int offset = first_lexeme_entries->at(i);
        if (is_left_expanded)
            offset--;

        LexemeSequence expanded(_text, offset, n, n1);
        if (!validate_expanded(expanded, candidate))
            continue;

        num_expanded += store_expanded(&expanded, candidate, false);
    }
    return num_expanded;
}

bool Extractor::validate_expanded(const LexemeSequence &expanded, const LexemeSequence &source) const
{
    if (!expanded.isValid())
        return false;

    if (_extracted->contains(*(expanded.key())))
        return false;

    return has_better_score(expanded, source);
}

bool Extractor::has_better_score(const LexemeSequence &expanded, const LexemeSequence &source) const
{
    // FIXME: zero expanded score?
    return expanded.score() > 0.0 && expanded.score() > source.score() - _qdt;
}

int Extractor::store_expanded(LexemeSequence *expanded, const LexemeSequence &source, bool is_left_expanded)
{
    expanded->incLeftExpansionDistance (source.leftExpansionDistance());
    expanded->incRightExpansionDistance(source.rightExpansionDistance());

    if (is_left_expanded)
        expanded->incLeftExpansionDistance();
    else
        expanded->incRightExpansionDistance();

    _candidates->append(*expanded);
    _extracted->insert (*(expanded->key()));

    return 1;
}

void Extractor::_initialize()
{
    _candidates = new QList<LexemeSequence>;
    _extracted  = new QSet<QByteArray>;
}

void Extractor::_destroy()
{
    delete _candidates;
    delete _extracted;
}
