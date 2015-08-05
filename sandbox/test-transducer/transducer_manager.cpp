#include "transducer_manager.h"

const qint8 STATE_MARK_FINAL     = 'f';
const qint8 STATE_MARK_NON_FINAL = 'F';

const qint32 TRANSDUCER_FORMAT_MARKER  = 0x51555444; // QUTD = Qubiq Util TransDucer
const qint32 TRANSDUCER_FORMAT_VERSION = 1;

TransducerManager::TransducerManager()
{
    t             = new Transducer();
    is_self_alloc = true;
}

TransducerManager::TransducerManager(Transducer *other)
{
    t             = other;
    is_self_alloc = false;
}

TransducerManager::~TransducerManager()
{
    if (is_self_alloc)
        delete t;
}

/*
 * SERIALIZED      := PROLOGUE STATES
 * PROLOGUE        := PROLOGUE_MARKER VERSION INIT_STATE_ID NUM_STATES
 * PROLOGUE_MARKER := 'Q' 'U' 'T' 'D'
 * VERSION         := qint32
 * INIT_STATE_ID   := qint64
 * NUM_STATES      := qint64
 * STATES          := (FINAL_STATE | NON_FINAL_STATE)*
 * FINAL_STATE     := STATE_ID 'f' FINAL_STRING*
 * NON_FINAL_STATE := STATE_ID 'F' NUM_TRANSITIONS TRANSITION*
 * STATE_ID        := qint64
 * FINAL_STRING    := qstring
 * NUM_TRANSITIONS := qint64
 * TRANSITION      := LABEL OUTPUT NEXT_STATE_ID
 * LABEL           := qchar
 * OUTPUT          := qstring
 * NEXT_STATE_ID   := qint64
 */

bool TransducerManager::saveTofile(const QString &fname)
{
    QFile out_file(fname);
    if (!out_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    QDataStream out_stream(&out_file);

    QHash<uint, State*> *states  = t->states;
    QHash<uint, State*>::const_iterator i_state;

    out_stream
        << TRANSDUCER_FORMAT_MARKER
        << TRANSDUCER_FORMAT_VERSION
        << (qint64)(t->init_state)
        << (qint64)(states->size())
    ;

    for (i_state = states->begin(); i_state != states->end(); ++i_state) {

        State *state = i_state.value();
        out_stream << (qint64)state;

        if (state->isFinal()) {
            out_stream
                << STATE_MARK_FINAL
                << *(state->finalStrings()) // FIXME: Rewrite as a string list
            ;
        } else {
            QVector<Transition*> *transitions = state->transitions();
            qint64 num_transtitions           = transitions->size();

            out_stream
                << STATE_MARK_NON_FINAL
                << num_transtitions
            ;
            for (int i = 0; i < num_transtitions; i++) {
                Transition *transition = transitions->at(i);
                State *next = transition->next();
                out_stream
                    << transition->label()
                    << transition->output()
                    << (qint64)next
                ;
            }
        }
    }

    out_file.close();
    return true;
}

bool TransducerManager::loadFromFile(const QString &fname)
{
    QFile in_file(fname);
    if (!in_file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in_stream(&in_file);

    qint32 format_marker  = 0;
    qint32 format_version = 0;
    qint64 init_state_id  = 0;
    qint64 num_states     = 0;

    in_stream
        >> format_marker
        >> format_version
        >> init_state_id
        >> num_states
    ;

    if (init_state_id == 0) {
        return false;
    }

    t->clear();

    QHash<uint, State*>   *states  = t->states;
    QHash<qint64, State*> *id2addr = new QHash<qint64, State*>();
    while (!in_stream.atEnd()) {
        qint64 state_id   = 0;
        qint8  state_mark = 0;

        in_stream >> state_id; // FIXME: validate state_id

        State *state = get_or_alloc_state(state_id, id2addr);

        in_stream >> state_mark;
        if (state_mark == STATE_MARK_FINAL) {
            state->setFinal(true);
            QVector<QString> finals;
            in_stream >> finals;
            for (int i = 0; i < finals.size(); i++) {
                state->addFinal(finals.at(i));
            }
        } else if (state_mark == STATE_MARK_NON_FINAL) {
            state->setFinal(false);

            qint64 num_transitions = 0;
            in_stream >> num_transitions; // FIXME: validate num_transitions

            for (int i = 0; i < num_transitions; i++) {
                QChar   label   = '\0';
                QString output  = "";
                qint64  next_id = 0;
                in_stream
                    >> label
                    >> output
                    >> next_id
                ;
                // FIXME: validate next_id and label

                State *next = get_or_alloc_state(next_id, id2addr);

                state->setNext(label, next);
                state->setOutput(label, output);
            }
            states->insert(state->key(), state);
        } else {
            // FIXME: throw an error
        }
    }

    State *init_state = id2addr->value(init_state_id, NULL);

    if (init_state == NULL) {
        return false;
    }
    // TESTME: Test initial state only transducers

    t->init_state = init_state;

    delete id2addr;
    in_file.close();

    return true;
}

bool TransducerManager::buildFromFile(const QString &fname)
{
    Q_UNUSED(fname);
    return true;
}

State* TransducerManager::get_or_alloc_state(qint64 state_id, QHash<qint64, State *> *id2addr) const
{
    State *state = id2addr->value(state_id, NULL);
    if (state != NULL) {
        return state;
    }

    State *new_state = new State();
    id2addr->insert(state_id, new_state);
    return new_state;
}
