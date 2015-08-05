#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <QtCore>
#include "state.h"

class TransducerManager;

class Transducer {
    friend class TransducerManager;

public:
    Transducer();

    ~Transducer();

    bool isReady() const { return init_state != NULL; }

    bool build(const QString &fname, int max_word_size);

    QStringList search(const QString &s) const;

private:
    QHash<uint, State*> *states;
    State               *init_state;
    QVector <State*>    *tmp_states;

    State *find_equivalent(const State *state);
    int common_prefix_length(const QString &s1, const QString &s2) const;
    QString common_prefix(const QString &s1, const QString &s2) const;

    void clear();

    void _initialize_tmp_states(int n);
    void _destroy_tmp_states();
};

#endif // _TRANSDUCER_H_
