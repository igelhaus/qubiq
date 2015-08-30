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
    bool isReady() const { return init_state != NULL; }

    QStringList search(const QString &s, TransducerSearchTrace *trace = NULL) const;

private:
    QList<State*> *states;     //!< List of states composing the transducer.
    State         *init_state; //!< The initial state of the transducer.

    static const QString _empty_string;

    void clear();
};

#endif // _TRANSDUCER_H_
