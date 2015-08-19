#include <qubiq/util/transducer_state.h>

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

State* State::next(const QChar &label) const
{
    Transition *t = _transitions->value(label, NULL);
    return t != NULL? t->next() : NULL;
}

void State::setNext(const QChar &label, State *next)
{
    Transition *t = _transitions->value(label, NULL);
    if (t == NULL) {
        t = new Transition(label);
        _transitions->insert(label, t);
    }
    t->setNext(next);
}

QString State::output(const QChar &label) const
{
    Transition *t = _transitions->value(label, NULL);
    return t != NULL? t->output() : QString();
}

void State::setOutput(const QChar &label, const QString &output)
{
    Transition *t = _transitions->value(label, NULL);
    if (t == NULL) {
        return;
    }
    t->setOutput(output);
}

void State::updateOutputsWithPrefix(const QString &prefix)
{
    QHash<QChar, Transition*>::iterator i_t;
    for (i_t = _transitions->begin(); i_t != _transitions->end(); ++i_t) {
        i_t.value()->prependOutput(prefix);
    }
}

void State::clear()
{
    _destroy_transitions();
    _final_suffixes->clear();
    is_final = false;
}

uint State::key(uint seed) const
{
    const int size_of_f_key  = sizeof(uint);
    const int size_of_t_key  = sizeof(uint);
    const int size_of_t_keys = size_of_t_key * _transitions->size();
    const int size_of_key    = 1 + size_of_t_keys + (is_final? size_of_f_key : 0);

    char *key        = new char[size_of_key + 1];
    key[0]           = is_final? 'f' : 'F';
    key[size_of_key] = '\0';

    // Ensure the same order of keys for hashing transitions
    QList<QChar> t_keys = _transitions->keys();
    std::sort(t_keys.begin(), t_keys.end());

    for (int i = 0; i < t_keys.size(); i++) {
        Transition *t = _transitions->value(t_keys.at(i));
        uint t_key    = qHash(*t, seed);
        const char *bytes_t_key = static_cast<const char*>(static_cast<const void*>(&t_key));
        std::copy(
            bytes_t_key,
            bytes_t_key + size_of_t_key - 1,
            key + 1 + i * size_of_t_key
        );
    }

    if (is_final) {
        uint f_key              = qHash(_final_suffixes->join('|'), seed);
        const char *bytes_f_key = static_cast<const char*>(static_cast<const void*>(&f_key));
        std::copy(
            bytes_f_key,
            bytes_f_key + size_of_f_key - 1,
            key + 1 + size_of_t_keys
        );
    }

    uint state_hash = qHash(key, seed);
    delete []key;

    return state_hash;
}

bool State::addFinal(const QString &final)
{
    const int num_finals = _final_suffixes->size();
    int pos = 0;
    for (int i = 0; i < num_finals; i++) {
        int cmp_result = final.compare(_final_suffixes->at(i), Qt::CaseSensitive);
        if (cmp_result == 0) {
            return false;
        }
        if (cmp_result > 0) {
            pos = i;
            break;
        }
    }
    _final_suffixes->insert(pos, final);
    return true;
}

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

void State::_initialize()
{
    is_final        = false;
    _transitions    = new QHash<QChar, Transition*>();
    _final_suffixes = new QStringList();
}

void State::_destroy()
{
    _destroy_transitions();
    delete _transitions;
    delete _final_suffixes;
}

void State::_assign(const State &other)
{
    is_final        = other.is_final;
    _final_suffixes = other._final_suffixes;

    _destroy_transitions(); // FIXME:
    QHash<QChar, Transition*>::const_iterator i_t;
    for (i_t = other._transitions->begin(); i_t != other._transitions->end(); ++i_t) {
        Transition *src_t = i_t.value();
        Transition *dst_t = new Transition(*src_t);
        _transitions->insert(i_t.key(), dst_t);
    }
}

void State::_destroy_transitions()
{
    qDeleteAll(_transitions->begin(), _transitions->end());
    _transitions->clear();
}
