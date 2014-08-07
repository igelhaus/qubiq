#include <qubiq/extractor.h>

Extractor::Extractor(const Text *text)
{
    _text    = text;
    _min_bf  = DEFAULT_MIN_BIGRAM_FREQUENCY;
    _max_ser = DEFAULT_MAX_SOURCE_EXTRACTION_RATE;
    _max_led = DEFAULT_MAX_LEFT_EXPANSION_DISTANCE;
    _max_red = DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE;
    _qdt     = DEFAULT_QUALITY_DECREASE_THRESHOLD;

    _initialize();
}

Extractor::~Extractor()
{
    _destroy();
}

bool Extractor::extract()
{
    LOG_INFO("Starting extraction");

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
        LOG_DEBUG("Expanding candidate: ", candidate.image(_text).toStdWString().data());
        LOG_DEBUG("Expansions made (left, right, total): ", num_expanded_left, num_expanded_right, num_expansions);

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

    return true;
}

bool Extractor::collect_good_bigrams()
{
    LOG_INFO("Starting collecting good bigrams");
    for (int i = 0; i < _text->length(); i++) {
        LexemeSequence bigram(_text, i, 2, 1);
        if (!bigram.isValid())
            continue;
        const QByteArray *key = bigram.key();
        if (!_extracted->contains(*key) && is_good_bigram(bigram)) {
            LOG_DEBUG("Found at offset ", i);
            _extracted->insert(*key);
            _candidates->append(bigram);
        }
    }
    LOG_INFO("Good bigrams collected");
    return _extracted->size() > 0;
}

bool Extractor::is_good_bigram(const LexemeSequence &bigram) const
{
    return bigram.frequency() >= _min_bf;
}

bool Extractor::treat_as_term(const LexemeSequence &candidate, int num_expansions) const
{
    // If most occurrences of a candidate in the corpus have not been extended then treat it as a term.
    double exp_ratio = (double)num_expansions / (double)candidate.frequency();
    LOG_DEBUG("Expansion ratio: ", exp_ratio);
    return exp_ratio <= _max_ser;
}

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
