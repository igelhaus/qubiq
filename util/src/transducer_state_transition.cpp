#include <qubiq/util/transducer_state_transition.h>

const QString Transition::_default_output = QString("");
const QString Transition::_output_sep     = QString("");

//! \internal Checks if output chains of \c this transition and \c other transition are equal by char.
bool Transition::has_equal_output(const Transition &other) const
{
    const QStringList &o1 = o;
    const QStringList &o2 = other.o;

    int o1_pos = 0;
    int o2_pos = 0;
    int o1_id  = 0;
    int o2_id  = 0;

    bool o1_ended      = false;
    bool o2_ended      = false;
    bool o1_comparable = false;
    bool o2_comparable = false;

    QChar c1;
    QChar c2;

    while (true) {
        if (!o1_comparable) {
            if (o1_id >= o1.size()) {
                o1_ended      = true;
                o1_comparable = true;
            } else {
                if (o1_pos >= o1.at(o1_id).length()) {
                    o1_id++;
                    o1_pos        = 0;
                    o1_comparable = false;
                } else {
                    c1            = o1.at(o1_id).at(o1_pos);
                    o1_comparable = true;
                    o1_pos++;
                }
            }
        }

        if (!o2_comparable) {
            if (o2_id >= o2.size()) {
                o2_ended      = true;
                o2_comparable = true;
            } else {
                if (o2_pos >= o2.at(o2_id).length()) {
                    o2_id++;
                    o2_pos        = 0;
                    o2_comparable = false;
                } else {
                    c2            = o2.at(o2_id).at(o2_pos);
                    o2_comparable = true;
                    o2_pos++;
                }
            }
        }

        if (!o1_comparable || !o2_comparable) {
            continue;
        }

        if (o1_ended != o2_ended) {
            // One of output chains has ended, the other one has not: not equal
            return false;
        }

        if (o1_ended == true) {
            // Both outputchains have the same length and equal payload: equal
            return true;
        }

        if (c1 != c2) {
            // At some point, corresponding characters are different: not equal
            return false;
        }

        // Corresponding characters are equal: fetch next pair for comparison
        o1_comparable = false;
        o2_comparable = false;
    }
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
    const uint    output_hash = qHash((const_cast<Transition&>(t)).output(), seed);

    char *key = new char[SIZE_OF_T_KEY];
    memcpy(key                               , &label_code , SIZE_OF_LABEL );
    memcpy(key + SIZE_OF_LABEL               , &next_addr  , SIZE_OF_NEXT  );
    memcpy(key + SIZE_OF_LABEL + SIZE_OF_NEXT, &output_hash, SIZE_OF_OUTPUT);

    uint transition_hash = qHash(QByteArray(key, SIZE_OF_T_KEY), seed);
    delete []key;

    return transition_hash;
}
