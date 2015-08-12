#include <qubiq/util/transducer.h>

Transducer::Transducer()
{
    states     = new QList<State*>();
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
            current_state = NULL;
            break;
        }
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

void Transducer::clear()
{
    qDeleteAll(states->begin(), states->end());
    states->clear();
    init_state = NULL;
}
