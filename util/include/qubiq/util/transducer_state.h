#ifndef _TRANSDUCER_STATE_H_
#define _TRANSDUCER_STATE_H_

#include <QtCore>
#include <QtAlgorithms>
#include <qubiq/util/transducer_state_transition.h>

class State {
public:
    State();
    State(const State &other);

    ~State();

    State& operator =(const State &other);

    inline bool isFinal() const      { return is_final; }
    inline void setFinal(bool final) { is_final = final; }

    State *next(const QChar &label) const;
    void setNext(const QChar &label, State *next);

    QString output(const QChar &label) const;
    void setOutput(const QChar &label, const QString &output);

    QHash<QChar, Transition*>* transitions() const { return _transitions; }

    void updateOutputsWithPrefix(const QString &prefix);

    void clear();

    uint key(uint seed = 0) const;

    const QList<QString>* finalStrings() const { return _final_suffixes; }

    bool addFinal(const QString &final);
    bool updateFinalsWithPrefix(const QString &prefix);

private:
    bool is_final;
    QHash<QChar, Transition*> *_transitions;
    QStringList               *_final_suffixes;

    void _initialize();
    void _destroy();
    void _assign(const State &other);
    void _destroy_transitions();
};

#endif // _TRANSDUCER_STATE_H_
