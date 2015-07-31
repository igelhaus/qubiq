#ifndef _STATE_H_
#define _STATE_H_

#include <QtCore>
#include <qubiq/util/transition.h>

class State {
public:
    State();
    State(const State &other);

    ~State();

    State& operator =(const State &other);

    inline bool isFinal() const      { return is_final; }
    inline void setFinal(bool final) { is_final = final; }

    State *next(const QChar &c);
    void setNext(const QChar &c, State *next);

    QString output(const QChar &c);
    void setOutput(const QChar &c, const QString &output);

    QVector<Transition*>* transitions() const { return _t; }

    void clear();

    uint key(uint seed = 0) const;

    const QVector<QString>* finalStrings() const { return finals; }

    bool updateFinalsWithPrefix(const QString &prefix);
    inline void addFinal(const QString &final) { finals->append(final); }

private:
    bool is_final;
    QVector<Transition*> *_t;
    QVector<QString>     *finals;

    Transition *transition_by_label(const QChar &c);

    char* final_state_key(uint seed = 0) const;
    char* non_final_state_key(uint seed = 0) const;

    void _initialize();
    void _destroy();
    void _assign(const State &other);
    void _destroy_transitions();
};

inline uint qHash(const State &state, uint seed) {
    return qHash(state.key(seed), seed);
}

#endif // _STATE_H_
