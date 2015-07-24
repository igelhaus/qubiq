#ifndef _STATE_H_
#define _STATE_H_

#include <QtCore>

class State;

class Transition {
public:
    Transition() { n = NULL; }
    Transition(const Transition &other) {
        o = other.o;
        l = other.l;
        n = other.n;
    }
    ~Transition() {}

    inline QChar label() const { return l; }
    inline void setLabel(const QChar &c) { l = c; }

    inline QString output() const { return o; }
    inline void setOutput(const QString &output) { o = output; }

    inline void prependOutput(const QString &prefix) { o.prepend(prefix); }

    inline State* next() const { return n; }
    inline void setNext(State *next) { n = next; }

private:

    QString o;
    QChar   l;
    State   *n;
};

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

    QString key() const;

    const QVector<QString>* finalStrings() const { return finals; }

    bool updateFinalsWithPrefix(const QString &prefix);
    inline void addFinal(const QString &final) { finals->append(final); }

private:
    bool is_final;
    QVector<Transition*> *_t;
    QVector<QString>     *finals;

    Transition *transition_by_label(const QChar &c);

    void _initialize();
    void _destroy();
    void _assign(const State &other);
    void _destroy_transitions();
};

inline uint qHash(const State &state, uint seed) {
    return qHash(state.key(), seed);
}

#endif // _STATE_H_
