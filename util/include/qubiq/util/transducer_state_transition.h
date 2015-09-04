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
    inline bool operator ==(const Transition &other) const
    {
        return n == other.n && l == other.l && has_equal_output(other);
    }

    //! Returns \c true if \c this transition is not equal to \c other and \c false otherwise.
    inline bool operator !=(const Transition &other) const { return !operator ==(other); }

    //! Returns a label associated with \c this transition.
    inline QChar label() const { return l; }

    //! Sets a label to be associated with \c this transition.
    inline void setLabel(const QChar &c) { l = c; }

    //! Returns a state \c this transition points to.
    inline State* next() const { return n; }

    //! Sets an state this transition should point to.
    inline void setNext(State *next) { n = next; }

    //! Returns an output associated with \c this transition.
    inline QString output()
    {
        if (o.size() == 0) {
            return QString(_default_output);
        }
        if (o.size() > 1) {
            _join_and_assign(NULL);
        }
        return o.at(0);
    }

    //! Sets an \c output to be associated with \c this transition.
    inline void setOutput(const QString &output) { o.clear(); o << output; }

    //! Chops \c n characters from the end of the first item of the transition chain.
    inline void chopOutput(int n) { if (o.size() > 0) { o[0].chop(n); } }

    //! Prepends a \c prefix to the output associated with \c this transition.
    inline void prependOutput(const QString &prefix) { o.prepend(prefix); }

private:

    QChar        l; //!< Label associated with \c this transition.
    State       *n; //!< State \c this transition points to.
    QStringList  o; //!< Output associated with \c this transition.

    //! \internal Assings \c other internals to \c this.
    inline void _assign(const Transition &other)
    {
        l = other.l;
        n = other.n;
        _join_and_assign(&other);
    }

    //! \internal Joins output chain into a single string and assigns it to the first element int he list
    inline void _join_and_assign(const Transition *other = NULL)
    {
        QString _o = other != NULL? other->o.join(_output_sep) : o.join(_output_sep);
        o.clear();
        o << _o;
    }

    bool has_equal_output(const Transition &other) const;

    static const QString _default_output;
    static const QString _output_sep;
};

//! Calculates the unique key of the transition \c t.
uint qHash(const Transition &t, uint seed = 0);

#endif // _TRANSDUCER_STATE_TRANSITION_H_
