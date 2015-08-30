#ifndef _TRANSDUCER_STATE_TRANSITION_H_
#define _TRANSDUCER_STATE_TRANSITION_H_

#include <string.h>
#include <QtCore>

class State;

/**
 * \class Transition
 *
 * \brief The Transition class implements a single transition for a transducer's state.
 *
 * \sa State
 * \sa Transducer
 */

class Transition {
public:
    //! Constructs a transition object with a given \c label.
    Transition(const QChar &label)
    {
        l = label;
        n = NULL;
    }

    //! Copy constructor. Constructs a Transition object copying it from the \c other transition.
    Transition(const Transition &other)
    {
        _assign(other);
    }

    //! Destructs the Transition object.
    ~Transition() {}

    //! Assignment operator. Assigns \c other transition to \c this transition and returns a reference to \c this transition.
    Transition& operator =(const Transition &other)
    {
        if (this != &other) {
            _assign(other);
        }
        return *this;
    }

    //! Comparison operator. Returns \c true if \c this transition is equal to \c other and \c false otherwise.
    inline bool operator ==(const Transition &other) const { return l == other.l && n == other.n && o == other.o; } // FIXME: test me

    //! Returns a label associated with \c this transition.
    inline QChar label() const { return l; }

    //! Sets a label to be associated with \c this transition.
    inline void setLabel(const QChar &c) { l = c; }

    //! Returns a state \c this transition points to.
    inline State* next() const { return n; }

    //! Sets an state this transition should point to.
    inline void setNext(State *next) { n = next; }

    //! Returns an output associated with \c this transition.
    inline QString output() const {
        if (o.size() == 0) {
            return QString(_default_output);
        } else if (o.size() == 1) {
            return o.at(0);
        } else {
            return o.join(_output_sep);
        }
    }

    //! Sets an output to be associated with \c this transition.
    inline void setOutput(const QString &output) { o.clear(); o << output; }

    //! Prepends a \c prefix to the output associated with \c this transition.
    inline void prependOutput(const QString &prefix) { o.prepend(prefix); }

private:

    QChar        l; //!< Label associated with \c this transition.
    QStringList  o; //!< Output associated with \c this transition.
    State       *n; //!< State \c this transition points to.

    //! \internal Assings \c other internals to \c this.
    void _assign(const Transition &other)
    {
        QString _o = other.o.join(_output_sep);
        o.clear();
        o << _o;
        l = other.l;
        n = other.n;
    }

    static const QString _default_output;
    static const QString _output_sep;
};

//! Calculates the unique key of the transition \c t.
uint qHash(const Transition &t, uint seed = 0);

#endif // _TRANSDUCER_STATE_TRANSITION_H_
