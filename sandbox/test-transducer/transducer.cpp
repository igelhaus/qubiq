#include "transducer.h"

Transducer::Transducer()
{
    states     = new QHash<uint, State*>();
    init_state = NULL;
}

Transducer::~Transducer()
{
    clear();
    delete states;
}

QStringList Transducer::search(const QString &s) const
{
    QStringList result;

    if (!isReady()) {
        return result;
    }

    QString output_prefix("");
    State *current_state = init_state;
    State *next_state    = NULL;
    for (int i = 0; i < s.length(); i++) {
        const QChar &c = s.at(i);
        next_state     = current_state->next(c);
        if (next_state == NULL) {
            qDebug() << "char" << c << ": fail";
            current_state = NULL;
            break;
        }
        qDebug() << "char" << c << ": found";
        output_prefix.append(current_state->output(c));
        current_state = next_state;
    }

    if (current_state == NULL) {
        return result;
    }

    if (!current_state->isFinal()) {
        return result;
    }

    const QVector<QString> *finals = current_state->finalStrings();
    result.append(output_prefix);
    for (int i = 0; i < finals->size(); i++) {
        result.append(output_prefix + finals->at(i));
    }

    return result;
}

State* Transducer::find_equivalent(const State *state)
{
    uint state_key = state->key();
    qDebug() << "state_key =" << state_key;
    if (states->contains(state_key)) {
        qDebug() << "contains";
        return states->value(state_key);
    }
    qDebug() << "does not contain";
    State *_state = new State(*state);
    states->insert(state_key, _state);
    return _state;
}

void Transducer::clear()
{
    QHash<uint, State*>::iterator state;
    for (state = states->begin(); state != states->end(); ++state) {
        delete state.value();
    }
    states->clear();
    init_state = NULL;
}
