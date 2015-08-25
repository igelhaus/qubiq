#include <qubiq/util/transducer.h>

const QString Transducer::_empty_string = QString("");

/**
 * \class Transducer
 *
 * \brief The Transducer class provides a data structure for fast token/lexeme recognition and
 * simultaneous retrieval of arbitrary data assosiated with them.
 *
 * \sa TransducerManager
 */

//! Constructs the Transducer object.
Transducer::Transducer()
{
    states     = new QList<State*>();
    init_state = NULL;
}

//! Destructs the Transducer object.
Transducer::~Transducer()
{
    clear();
    delete states;
}

/**
 * \brief Searches the transducer.
 *
 * This is the main interface of the class. It reads the input token/lexeme \c s char by char
 * and tries to find the same trace in the transducer gathering all data associated with \c s.
 * If \c s is found, this data is returned in the for of the string list. Otherwise an empty list
 * is returned.
 *
 * \param[in] s A token/lexeme to search.
 *
 * \returns A string list of outputs.
 */
QStringList Transducer::search(const QString &s) const
{
    QStringList result;

    if (!isReady()) {
        return result;
    }

    QStringList output_prefix_parts(Transducer::_empty_string);
    State *current_state = init_state;
    State *next_state    = NULL;
    for (int i = 0; i < s.length(); i++) {
        const QChar &c = s.at(i);
        next_state     = current_state->next(c);
        if (next_state == NULL) {
            current_state = NULL;
            break;
        }
        const QString _output = current_state->output(c);
        if (_output.length() > 0) {
            output_prefix_parts.append(_output);
        }
        current_state = next_state;
    }

    if (current_state == NULL) {
        return result;
    }

    if (!current_state->isFinal()) {
        return result;
    }

    // NB! The join below seems to be very expensive. Consider adding caching for search.
    const QString output_prefix  = output_prefix_parts.join(Transducer::_empty_string);
    const QList<QString> *finals = current_state->finalStrings();
    result.append(output_prefix); // FIXME: test this
    for (int i = 0; i < finals->size(); i++) {
        result.append(output_prefix + finals->at(i));
    }

    return result;
}

//! Clears the Transducer object: All memory allocated for storing states gets destroyed, but the containers remain undestroyed.
void Transducer::clear()
{
    qDeleteAll(states->begin(), states->end());
    states->clear();
    init_state = NULL;
}
