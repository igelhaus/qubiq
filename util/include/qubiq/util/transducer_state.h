#ifndef _TRANSDUCER_STATE_H_
#define _TRANSDUCER_STATE_H_

#include <string.h>
#include <algorithm>

#include <QtCore>
#include <QtAlgorithms>
#include <qubiq/util/transducer_state_transition.h>

class State {
public:
    State();
    State(const State &other);

    ~State();

    State& operator =(const State &other);

    //! Returns \c if state is final for some trace, \c false otherwise.
    inline bool isFinal() const { return is_final; }

    //! Sets finality flag for the state.
    inline void setFinal(bool final) { is_final = final; }

    //! Returns pointer to the hash of transitions from \c this state.
    inline QHash<QChar, Transition*>* transitions() const { return _transitions; }

    //! Returns pointer to the list of final suffixes of \c this state.
    inline const QList<QString>* finalStrings() const { return _final_suffixes; }

    State *next   (const QChar &label) const;
    void   setNext(const QChar &label, State *next);

    QString output   (const QChar &label) const;
    void    setOutput(const QChar &label, const QString &output);

    void updateOutputsWithPrefix(const QString &prefix);

    bool addFinal              (const QString &final);
    bool updateFinalsWithPrefix(const QString &prefix);

    void clear();

    uint key(uint seed = 0) const;

private:
    bool is_final; //!< Flag: \c true if state is final for some trace, \c false otherwise.
    QHash<QChar, Transition*> *_transitions;     //!< Hash of transitions from \c this state.
    QStringList               *_final_suffixes;  //!< List of final suffixes; relevant if \c this state is final for some trace.

    void _initialize();
    void _assign(const State &other);
};

#endif // _TRANSDUCER_STATE_H_
