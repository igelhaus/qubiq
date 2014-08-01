#include <qubiq/extractor.h>

Extractor::Extractor(const Text *text)
{
    _text    = text;
    _min_bf  = DEFAULT_MIN_BIGRAM_FREQUENCY;
    _ber     = DEFAULT_BIGRAM_EXTRACTION_RATE;
    _max_led = DEFAULT_MAX_LEFT_EXPANSION_DISTANCE;
    _max_red = DEFAULT_MAX_RIGHT_EXPANSION_DISTANCE;
    _qdt     = DEFAULT_QUALITY_DECREASE_THRESHOLD;

    _candidates = new QList<LexemeSequence>;
    _extracted  = new QSet<QByteArray>;
}

Extractor::~Extractor()
{
    delete _candidates;
    delete _extracted;
}

bool Extractor::extract()
{
    if (!collect_good_bigrams())
        return false;
    int i = 0;

    // FIXME: add method for fetching led and red

    while (i < _candidates->size()) {
        LexemeSequence &candidate = _candidates->at(i);
        int num_expanded_left  = 0;
        int num_expanded_right = 0;
        int num_expansions     = 0;

        if (candidate.leftExpansionDistance() < _max_led)
            num_expanded_left = expand_left(i);

        if (candidate.rightExpansionDistance() < _max_red)
            num_expanded_right = expand_right(i);

        num_expansions = num_expanded_left + num_expanded_right;
        if (num_expansions > 0) {
            // At least one expansion done
            if (treat_bigram_as_term(i, num_expansions)) {
                i++;
            } else {
                _extracted->remove(*(candidate.sequenceKey()));
                _candidates->removeAt(i);
            }
        } else {
            // No expansion: simply leave current candidate as a term
            i++;
        }
    }

    return true;
}

bool Extractor::collect_good_bigrams()
{
    for (int i = 0; i < _text->length(); i++) {
        LexemeSequence bigram(_text, i, 2, 1);
        if (!bigram.isValid())
            continue;
        QByteArray *seq_key = bigram.sequenceKey();
        if (!_extracted->contains(*seq_key) && is_good_bigram(bigram)) {
            _extracted->insert(*seq_key);
            _candidates->append(bigram);
        }
    }
    return _extracted->size() > 0;
}

bool Extractor::is_good_bigram(const LexemeSequence &bigram)
{
    // FIXME: add method for bigram frequency
    return bigram.frequency() >= _min_bf;
}

// FIXME: if most occurrences of c in the corpus have not been extended then add c to E.
bool Extractor::treat_bigram_as_term(int i, int num_expansions)
{
    // return num_expansions / frequency <= _ber;
    return true;
}

int Extractor::expand_left(int i)
{

}

int Extractor::expand_right(int i)
{

}
