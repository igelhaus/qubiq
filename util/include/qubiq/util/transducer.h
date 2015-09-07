#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <QtCore>
#include <QtAlgorithms>
#include <qubiq/util/transducer_state.h>

class TransducerManager;

struct TransducerSearchTrace {
public:
    int     reached_pos;
    bool    is_transducer_ready;
    bool    is_reached_pos_final;
    QString labels_at_failed;
};

class Transducer {
    friend class TransducerManager;

public:
    Transducer();

    ~Transducer();

    //! Checks the state of the transducer and returns \c true it is ready for searching and \c false otherwise.
    inline bool isReady() const { return init_state != NULL; }

    QStringList search(const QString &s, TransducerSearchTrace *trace = NULL) const;

private:
    QSet<State>           *states_set;  //!< Set of states composing the transducer.
    QHash<qint64, State*> *states_hash; //!< Hash of states composing the transducer.
    State                 *init_state;  //!< The initial state of the transducer.

    static const QString _empty_string;

    //! Clears the Transducer object: All memory allocated for storing states gets destroyed, but the containers remain undestroyed.
    inline void clear()
    {
        init_state = NULL;

        qDeleteAll(states_hash->begin(), states_hash->end());

        states_set->clear();
        states_hash->clear();
    }

};

#endif // _TRANSDUCER_H_
