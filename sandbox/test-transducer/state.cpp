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

QString State::key() const
{
    if (_t->size() == 0) {
        return QString("");
    }

    QString k(is_final? '1' : '0');
    k.append(KEY_DELIMITER).append(QChar(_t->size()));

    for (int i = 0; i < _t->size(); i++) {
        Transition *t = _t->at(i);
        k.append(KEY_DELIMITER)
            .append(t->label())
            .append(KEY_DELIMITER)
            .append(t->output());
    }

    return k;
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
