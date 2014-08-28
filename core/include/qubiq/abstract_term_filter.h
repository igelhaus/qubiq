#ifndef _ABSTRACT_TERM_FILTER_H_
#define _ABSTRACT_TERM_FILTER_H_

#include <qubiq/lexeme_sequence.h>

class AbstractTermFilter {

public:
    AbstractTermFilter() {}
    virtual ~AbstractTermFilter() {}

    virtual bool passes(const LexemeSequence &sequence) = 0;
};

#endif // _ABSTRACT_TERM_FILTER_H_
