#include "transducer.h"

const char *KEY_DELIMITER = "|";

State::State()
{
    _initialize();
}

State::State(const State &other)
{
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
        return;
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
    _t->resize(0);
    is_final = false;
}

QString State::key() const
{
    if (_t->size() == 0) {
        return QString("");
    }

    QString k(QChar(_t->size()));

    for (int i = 0; i < _t->size(); i++) {
        Transition *t = _t->at(i);
        k.append(KEY_DELIMITER)
            .append(t->label())
            .append(KEY_DELIMITER)
            .append(t->output());
    }

    return k;
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
}

void State::_destroy()
{
    _destroy_transitions();
    delete _t;
}

void State::_destroy_transitions()
{
    for (int i = 0; i < _t->size(); i++)
        delete _t->at(i);
}

void State::_assign(const State &other)
{
    is_final = other.is_final;
    // FIXME: Copy transitions
}
