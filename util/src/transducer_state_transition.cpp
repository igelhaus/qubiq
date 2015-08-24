#include <qubiq/util/transducer_state_transition.h>

const QString Transition::_output_sep = QString("");

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
