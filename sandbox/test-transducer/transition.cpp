#include "transition.h"

Transition::Transition() {
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

QByteArray Transition::key() const
{
    // FIXME: implement this
//    int size_of_key =
//          sizeof(ushort)
//        + sizeof(qintptr)
//        + sizeof(uint)
//    ;
//    ushort  label_code  = l.unicode();
//    qintptr next_addr   = (qintptr)n;
//    uint    output_hash = qHash(o);

//    for (int i = 0; i != sizeof(lexeme_key); i++) {
//        int  shift = i << 3;
//        int  mask  = ((int)0xFF) << shift;
//        char byte  = (char)((lexeme_key & mask) >> shift);
//        _key->append(byte);
//    }

}

void Transition::_assign(const Transition &other)
{
    o = other.o;
    l = other.l;
    n = other.n;
}
