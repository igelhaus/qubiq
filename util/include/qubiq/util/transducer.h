#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <QtCore>
#include <qubiq/util/transducer_state.h>

class TransducerManager;

class Transducer {
    friend class TransducerManager;

public:
    Transducer();

    ~Transducer();

    bool isReady() const { return init_state != NULL; }
    QStringList search(const QString &s) const;

private:
    QHash<uint, State*> *states;
    State               *init_state;
    QVector <State*>    *tmp_states;

    State *find_equivalent(const State *state);

    void clear();

    void _initialize_tmp_states(int n);
    void _destroy_tmp_states();
};

#endif // _TRANSDUCER_H_
