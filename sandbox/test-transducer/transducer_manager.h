#ifndef TRANSDUCER_MANAGER_H
#define TRANSDUCER_MANAGER_H

#include <QtCore>
#include "transducer.h"

class TransducerManager
{
public:
    TransducerManager();
    TransducerManager(Transducer *other);
    ~TransducerManager();

    inline Transducer *transducer() const { return t; }

    bool buildFromFile(const QString &fname);
    bool saveTofile(const QString &fname);
    bool loadFromFile(const QString &fname);

private:
    bool is_self_alloc;
    Transducer *t;

    State *get_or_alloc_state(qint64 state_id, QHash<qint64, State*> *id2addr) const;
};

#endif // TRANSDUCER_MANAGER_H
