#include "state.h"

const char *KEY_DELIMITER = "|";

State::State()
{
    _initialize();
}

State::State(const State &other)
{
    _initialize();
    _assign(other);
}

State::~State()
{
    _destroy();
}

State& State::operator =(const State &other)
{
    if (this != &other) {
        _destroy();
        _assign(other);
    }
    return *this;
}

State* State::next(const QChar &c)
{
    Transition *t = transition_by_label(c);
    return t != NULL? t->next() : NULL;
}

void State::setNext(const QChar &c, State *next)
{
    Transition *t = transition_by_label(c);
    if (t == NULL) {
        t = new Transition();
        t->setLabel(c);
        _t->append(t);
    }
    t->setNext(next);
}

QString State::output(const QChar &c)
{
    Transition *t = transition_by_label(c);
    return t != NULL? t->output() : QString();
}

void State::setOutput(const QChar &c, const QString &output)
{
    Transition *t = transition_by_label(c);
    if (t == NULL) {
        return;
    }
    t->setOutput(output);
}

void State::clear()
{
    _destroy_transitions();
    finals->resize(0);
    is_final = false;
}

uint State::key(uint seed) const
{
    char *key;

    if (is_final) {
        key = final_state_key(seed);
    } else {
        key = non_final_state_key(seed);
    }

    uint state_hash = qHash(key, seed);
    delete []key;

    return state_hash;
}

bool State::updateFinalsWithPrefix(const QString &prefix)
{
    if (finals->size() == 0) { // FIXME: Is this correct?
        return false;
    }
    for (int i = 0; i < finals->size(); i++) {
        finals[i].prepend(prefix);
    }
    return true;
}

Transition* State::transition_by_label(const QChar &c)
{
    for (int i = 0; i < _t->size(); i++) {
        if (_t->at(i)->label() == c) {
            return _t->at(i);
        }
    }
    return NULL;
}

// Format: final-byte-mark | f_hash
char* State::final_state_key(uint seed) const
{
    const int size_of_f_key = sizeof(uint);
    const int size_of_key   = 1 + size_of_f_key;

    char *key        = new char[size_of_key + 1];
    key[0]           =  'f'; // final-byte-mark
    key[size_of_key] = '\0';

    QString all_finals;
    for (int i = 0; i < finals->size(); i++) {
        all_finals.append(finals->at(i)).append('|');
    }

    uint f_key = qHash(all_finals, seed);

    const char *bytes_f_key = static_cast<const char*>(static_cast<const void*>(&f_key));
    std::copy(
        bytes_f_key,
        bytes_f_key + size_of_f_key - 1,
        key + 1
    );

    return key;
}

// Format: non-final-byte-mark | t1_hash | t2_hash ...
char* State::non_final_state_key(uint seed) const
{
    const int size_of_t_key = sizeof(uint);
    const int size_of_key   = 1 + size_of_t_key * _t->size();

    char *key        = new char[size_of_key + 1];
    key[0]           =  'F'; // non-final-byte-mark
    key[size_of_key] = '\0';

    for (int i = 0; i < _t->size(); i++) {
        Transition *t = _t->at(i);
        uint t_key    = qHash(*t, seed);
        const char *bytes_t_key = static_cast<const char*>(static_cast<const void*>(&t_key));
        std::copy(
            bytes_t_key,
            bytes_t_key + size_of_t_key - 1,
            key + 1 + i * size_of_t_key
        );
    }

    return key;
}

void State::_initialize()
{
    is_final = false;
    _t       = new QVector<Transition*>();
    finals   = new QVector<QString>();
}

void State::_destroy()
{
    _destroy_transitions();
    delete _t;
    delete finals;
}

void State::_assign(const State &other)
{
    is_final = other.is_final;

    if (finals->size() > 0) {
        finals->clear();
    }

    for (int i = 0; i < other.finals->size(); i++) {
        finals->append(other.finals->at(i));
    }

    _destroy_transitions();
    for (int i = 0; i < other._t->size(); i++) {
        Transition *src_t = other._t->at(i);
        Transition *dst_t = new Transition(*src_t);
        _t->append(dst_t);
    }
}

void State::_destroy_transitions()
{
    for (int i = 0; i < _t->size(); i++) {
        delete _t->at(i);
    }
    _t->clear();
}
