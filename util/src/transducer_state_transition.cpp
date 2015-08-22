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

const int SIZE_OF_LABEL  = sizeof(ushort);
const int SIZE_OF_NEXT   = sizeof(quintptr);
const int SIZE_OF_OUTPUT = sizeof(uint);
const int SIZE_OF_T_KEY  = SIZE_OF_LABEL + SIZE_OF_NEXT + SIZE_OF_OUTPUT;
uint qHash(const Transition &t, uint seed)
{
    const ushort  label_code  = t.label().unicode();
    const qintptr next_addr   = (qintptr)(t.next());
    const uint    output_hash = qHash(t.output(), seed);

    char *key = new char[SIZE_OF_T_KEY];
    memcpy(key                               , &label_code , SIZE_OF_LABEL );
    memcpy(key + SIZE_OF_LABEL               , &next_addr  , SIZE_OF_NEXT  );
    memcpy(key + SIZE_OF_LABEL + SIZE_OF_NEXT, &output_hash, SIZE_OF_OUTPUT);

    uint transition_hash = qHash(QByteArray(key, SIZE_OF_T_KEY), seed);
    delete []key;

    return transition_hash;
}
