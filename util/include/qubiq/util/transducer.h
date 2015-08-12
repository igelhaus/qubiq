#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <QtCore>
#include <QtAlgorithms>
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
    QList<State*> *states;
    State         *init_state;

    void clear();
};

#endif // _TRANSDUCER_H_
