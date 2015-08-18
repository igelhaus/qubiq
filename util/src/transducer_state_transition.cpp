#include <qubiq/util/transducer_state_transition.h>

Transition::Transition(const QChar &label) {
    l = label;
    n = NULL;
}

Transition::Transition(const Transition &other) {
    _assign(other);
}

Transition::~Transition() {}

Transition& Transition::operator =(const Transition &other)
{
    if (this != &other) {
        _assign(other);
    }
    return *this;
}

void Transition::_assign(const Transition &other)
{
    o = other.o;
    l = other.l;
    n = other.n;
}

//
// Related functions
//

uint qHash(const Transition &t, uint seed)
{
    const int size_of_label  = sizeof(ushort);
    const int size_of_next   = sizeof(quintptr);
    const int size_of_output = sizeof(uint);
    const int size_of_key    = size_of_label + size_of_next + size_of_output;

    const ushort  label_code  = t.label().unicode();
    const qintptr next_addr   = (qintptr)(t.next()); // Test me: NULL
    const uint    output_hash = qHash(t.output(), seed);

    const char *bytes_label_code  = static_cast<const char*>(static_cast<const void*>(&label_code));
    const char *bytes_next_addr   = static_cast<const char*>(static_cast<const void*>(&next_addr));
    const char *bytes_output_hash = static_cast<const char*>(static_cast<const void*>(&output_hash));

    char *key        = new char[size_of_key + 1];
    key[size_of_key] = '\0';

    std::copy(bytes_label_code , bytes_label_code  + size_of_label  - 1, key);
    std::copy(bytes_next_addr  , bytes_next_addr   + size_of_next   - 1, key + size_of_label);
    std::copy(bytes_output_hash, bytes_output_hash + size_of_output - 1, key + size_of_label + size_of_next);

    uint transition_hash = qHash(key, seed);

    delete []key;

    return transition_hash;
}
