#include <qubiq/util/transducer_state.h>

/**
 * \class State
 *
 * \brief The State class implements a single state of a transducer.
 *
 * \sa Transducer
 */

//! Constructs a State object.
State::State()
{
    _initialize();
}

//! Copy constructor. Constructs a State object copying it from the \c other state.
State::State(const State &other)
{
    _initialize();
    _assign(other);
}

//! Destructs the State object.
State::~State()
{
    clear();
    delete _transitions;
    delete _final_suffixes;
}

//! Assignment operator. Assigns \c other state to \c this state and returns a reference to \c this state.
State& State::operator =(const State &other)
{
    if (this != &other) {
        clear();
        _assign(other);
    }
    return *this;
}

/**
 * @brief Comparison operator.
 *
 * Two states are equal if and only if they are both final or both non-final,
 * have equal lists of final suffixes and equal sets of transitions.
 *
 * @param[in] other State to compare \c this to.
 *
 * @return \c true if \c this state is equal to \c other and \c false otherwise.
 */
bool State::operator ==(const State &other) const // FIXME: Test me
{
    if (this == &other) {
        return true;
    }

    if (is_final != other.is_final) {
        return false;
    }

    if (is_final && *_final_suffixes != *(other._final_suffixes)) {
        return false;
    }

    if (_transitions->size() != other._transitions->size()) {
        return false;
    }

    QHash<QChar, Transition*>::const_iterator i;
    for (i = _transitions->constBegin(); i != _transitions->constEnd(); ++i) {
        Transition *this_t  = i.value();
        Transition *other_t = other._transitions->value(i.key(), NULL);
        if (other_t == NULL || *this_t != *other_t) {
            return false;
        }
    }

    return true;
}

//! Returns the next state for the \c label or \c NULL if there is no such transition.
State* State::next(const QChar &label) const
{
    Transition *t = _transitions->value(label, NULL);
    return t != NULL? t->next() : NULL;
}

//! Sets the \c next state for the \c label. If there is no transition for \c label, it is implicitly created.
void State::setNext(const QChar &label, State *next)
{
    Transition *t = _transitions->value(label, NULL);
    if (t == NULL) {
        t = new Transition(label);
        _transitions->insert(label, t);
    }
    t->setNext(next);
}

//! Returns the output for the \c label or an empty string if there is no such transition.
QString State::output(const QChar &label) const
{
    Transition *t = _transitions->value(label, NULL);
    return t != NULL? t->output() : QString();
}

//! Sets the \c output for the \c label. If there is no transition for \c label, it is \b not created: the method does nothing.
void State::setOutput(const QChar &label, const QString &output)
{
    Transition *t = _transitions->value(label, NULL);
    if (t == NULL) {
        return;
    }
    t->setOutput(output);
}

void State::chopOutput(const QChar &label, int n)
{
    Transition *t = _transitions->value(label, NULL);
    if (t == NULL) {
        return;
    }
    t->chopOutput(n);
}

//! Updates all outputs of the state with a given \c prefix.
void State::updateOutputsWithPrefix(const QString &prefix)
{
    QHash<QChar, Transition*>::iterator i_t;
    for (i_t = _transitions->begin(); i_t != _transitions->end(); ++i_t) {
        i_t.value()->prependOutput(prefix);
    }
}

//! Adds a new final suffix to the list of final suffixes keeping the list sorted in ascending order.
//! Returns \c false if \c final is already in the list, and \c true otherwise.
bool State::addFinal(const QString &final)
{
    const int num_finals = _final_suffixes->size();
    int insertion_pos    = num_finals;
    for (int i = 0; i < num_finals; i++) {
        int cmp_result = QString::compare(final, _final_suffixes->at(i), Qt::CaseSensitive);
        if (cmp_result == 0) {
            return false;
        } else if (cmp_result < 0) {
            insertion_pos = i;
            break;
        }
    }
    _final_suffixes->insert(insertion_pos, final);
    return true;
}

//! Updates all final suffixes of the state with a given \c prefix.
bool State::updateFinalsWithPrefix(const QString &prefix)
{
    if (_final_suffixes->size() == 0) {
        _final_suffixes->append(prefix);
    } else {
        for (int i = 0; i < _final_suffixes->size(); i++) {
            (*_final_suffixes)[i].prepend(prefix);
        }
    }
    return true;
}

//! Clears the state \b without destroying internals completely.
void State::clear()
{
    is_final = false;
    _final_suffixes->clear();
    qDeleteAll(_transitions->begin(), _transitions->end());
    _transitions->clear();
}

const int SIZE_OF_F_KEY = sizeof(uint);
const int SIZE_OF_T_KEY = sizeof(uint);

//! Calculates the unique key of \c this state.
uint State::key(uint seed) const
{
    const int num_transitions = _transitions->size();
    const int size_of_t_keys  = SIZE_OF_T_KEY * num_transitions;
    const int size_of_key     = 1 + size_of_t_keys + (is_final? SIZE_OF_F_KEY : 0);

    char *key = new char[size_of_key];
    key[0]    = is_final? 'f' : 'F';

    if (num_transitions > 0) {
        // Ensure the same order of keys for hashing transitions
        QList<QChar> t_keys = _transitions->keys();
        std::sort(t_keys.begin(), t_keys.end());
        for (int i = 0; i < num_transitions; i++) {
            uint t_key = qHash(*(_transitions->value(t_keys.at(i))), seed);
            memcpy(key + 1 + i * SIZE_OF_T_KEY, &t_key, SIZE_OF_T_KEY);
        }
    }

    if (is_final) {
        uint f_key = qHash(_final_suffixes->join('|'), seed);
        memcpy(key + 1 + size_of_t_keys, &f_key, SIZE_OF_F_KEY);
    }

    uint state_hash = qHash(QByteArray(key, size_of_key), seed);
    delete []key;

    return state_hash;
}

//! \internal Initializes object's internals.
void State::_initialize()
{
    is_final        = false;
    _transitions    = new QHash<QChar, Transition*>();
    _final_suffixes = new QStringList();
}

//! \internal Assings \c other internals to \c this.
void State::_assign(const State &other)
{
    is_final = other.is_final;

    if (is_final && other._final_suffixes->size() > 0) {
        *_final_suffixes = *(other._final_suffixes);
    }

    if (other._transitions->size() > 0) {
        QHash<QChar, Transition*>::const_iterator i_t;
        for (i_t = other._transitions->begin(); i_t != other._transitions->end(); ++i_t) {
            Transition *src_t = i_t.value();
            Transition *dst_t = new Transition(*src_t);
            _transitions->insert(i_t.key(), dst_t);
        }
    }
}
