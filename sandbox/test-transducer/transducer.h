#ifndef _TRANSDUCER_H_
#define _TRANSDUCER_H_

#include <QtCore>

class State;

class Transition {
public:
    Transition() { n = NULL; }
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

    bool updateFinalsWithPrefix(const QString &prefix);
    inline void addFinal(const QString &final) { finals->append(final); }

private:
    bool is_final;
    QVector<Transition*> *_t;
    QVector<QString>     *finals;

    Transition *transition_by_label(const QChar &c);

    void _initialize();
    void _destroy();
    void _destroy_transitions();
    void _assign(const State &other);
};

inline uint qHash(const State &state, uint seed) {
    return qHash(state.key(), seed);
}

class Transducer {
public:
    Transducer();

    ~Transducer();

    bool build(const QString &fname, int max_word_size);
    State *findEquivalent(const State *state);

private:
    QHash<QString, State*> *states;
    State *init_state;

    QVector <State*> *tmp_states;

    int common_prefix_length(const QString &s1, const QString &s2) const;
    QString common_prefix(const QString &s1, const QString &s2) const;
};


#endif // _TRANSDUCER_H_
