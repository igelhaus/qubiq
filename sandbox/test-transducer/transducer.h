#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <QtCore>
#include "state.h"

class Transducer {
public:
    Transducer();

    ~Transducer();

    bool build(const QString &fname, int max_word_size);
    State *findEquivalent(const State *state);

private:
    QHash<QString, State*> *states;
    State *init_state;

    QVector <State*> *tmp_states;

    int common_prefix_length(const QString &s1, const QString &s2) const;
    QString common_prefix(const QString &s1, const QString &s2) const;
};


#endif // _TRANSDUCER_H_
