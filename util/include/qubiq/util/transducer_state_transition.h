#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include <QtCore>

class State;

class Transition {
public:
    Transition();
    Transition(const Transition &other);

    ~Transition();

    Transition& operator =(const Transition &other);

    inline QChar label() const { return l; }
    inline void setLabel(const QChar &c) { l = c; }

    inline QString output() const { return o; }
    inline void setOutput(const QString &output) { o = output; }

    inline void prependOutput(const QString &prefix) { o.prepend(prefix); }

    inline State* next() const { return n; }
    inline void setNext(State *next) { n = next; }

    uint key(uint seed = 0) const;

private:

    QString o;
    QChar   l;
    State   *n;

    void _assign(const Transition &other);
};

uint qHash(const Transition &t, uint seed = 0);

#endif // _TRANSITION_H_
