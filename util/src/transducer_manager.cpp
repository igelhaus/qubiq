#include <qubiq/util/transducer_manager.h>

const int DEFAULT_MAX_WORD_SIZE = 1024;

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
 * Example state-trnasition chains:
 *
 * 0-w-0-o-0-r-0-d-0
 * 0-w-0-o-0-r-0-m-0
 *
 */
bool TransducerManager::build(const QString &fname, int max_word_size /*= 0*/)
{
    clear_err_str();

    QFile in_file(fname);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return set_err_str("Unable to open input file for reading");
    }

    if (max_word_size < 1) {
        max_word_size = DEFAULT_MAX_WORD_SIZE;
    }

    t->clear();
    QVector<State*> *tmp_states = TransducerManager::_initialize_tmp_states(max_word_size);

    qDebug() << "File successfully open, started building";

    QString     current_word;
    QString     current_output;
    QString     previous_word("");
    QTextStream in_stream(&in_file);
    while (!in_stream.atEnd()) {
        QString line      = in_stream.readLine();
        QStringList parts = line.split("\t");
        current_word    = parts.at(0);
        current_output  = parts.at(1);
        int current_len = current_word.length();

        qDebug() << "previous_word  =" << previous_word;
        qDebug() << "current_word   =" << current_word;
        qDebug() << "current_output =" << current_output;

        int prefix_len = TransducerManager::common_prefix_length(previous_word, current_word);
        qDebug() << "prefix_len =" << prefix_len;

        // We minimize the states from the suffix of the previous word
        for (int i = previous_word.length() /*= last previous state index*/; i >= prefix_len + 1; i--) {
            tmp_states->at(i - 1)->setNext(
                previous_word.at(i - 1),
                t->find_equivalent(tmp_states->at(i))
            );
        }
        // This loop intializes the tail states for the current word
        for (int i = prefix_len + 1; i <= current_len /*= last current state index*/; i++) {
            tmp_states->at(i)->clear();
            tmp_states->at(i - 1)->setNext(
                current_word.at(i - 1),
                tmp_states->at(i)
            );
        }
        if (previous_word != current_word) {
            tmp_states->at(current_len)->setFinal(true);
            // FIXME: Do we need to mark outputs of the final state somehow?
        }
        // Optimize output:
        for (int i = 0; i < prefix_len; i++) {
            QString _output       = tmp_states->at(i)->output(current_word.at(i)); // FIXME: ref?
            QString output_prefix = TransducerManager::common_prefix(_output, current_output);
            QString output_suffix = _output.right(_output.length() - output_prefix.length());

            tmp_states->at(i)->setOutput(current_word.at(i), output_prefix);

            QVector<Transition*> *transitions = tmp_states->at(i)->transitions();
            for (int j = 0; j < transitions->size(); j++) {
                transitions->at(j)->prependOutput(output_suffix);
            }
            if (tmp_states->at(i)->isFinal()) {
                tmp_states->at(i)->updateFinalsWithPrefix(output_suffix);
            }
            current_output = current_output.right(current_output.length() - output_prefix.length());
        }

        if (current_word == previous_word) {
            tmp_states->at(current_len)->addFinal(current_output);
        } else {
            tmp_states->at(prefix_len)->setOutput(current_word.at(prefix_len), current_output);
        }

        previous_word = current_word;
    }
    in_file.close();
    qDebug() << "List read";
    qDebug() << "Last word:" << current_word;

    // Minimize last word
    for (int i = current_word.length() /*= last previous state index*/; i >= 1; i--) {
        tmp_states->at(i - 1)->setNext(
            current_word.at(i - 1),
            t->find_equivalent(tmp_states->at(i))
        );
    }
//    qDebug() << "Last word minimized";

    t->init_state = t->find_equivalent(tmp_states->at(0));

    TransducerManager::_destroy_tmp_states(tmp_states);

//    qDebug() << "Built";
    return true;
}

/*
 * SERIALIZED      := PROLOGUE STATES
 * PROLOGUE        := PROLOGUE_MARKER VERSION INIT_STATE_ID NUM_STATES
 * PROLOGUE_MARKER := 'Q' 'U' 'T' 'D'
 * VERSION         := qint32
 * INIT_STATE_ID   := qint64
 * NUM_STATES      := qint64
 * STATES          := (FINAL_STATE | NON_FINAL_STATE)*
 * FINAL_STATE     := STATE_ID 'f' FINAL_STRING* TRANSITIONS
 * NON_FINAL_STATE := STATE_ID 'F' TRANSITIONS
 * STATE_ID        := qint64
 * FINAL_STRING    := qstring
 * TRANSITIONS     := NUM_TRANSITIONS TRANSITION*
 * NUM_TRANSITIONS := qint64
 * TRANSITION      := LABEL OUTPUT NEXT_STATE_ID
 * LABEL           := qchar
 * OUTPUT          := qstring
 * NEXT_STATE_ID   := qint64
 */

bool TransducerManager::save(const QString &fname)
{
    clear_err_str();

    if (!t->isReady()) {
        return set_err_str("Unable to save unready transducer");
    }


    QFile out_file(fname);
    if (!out_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return set_err_str("Unable to open output file for writing");
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
            out_stream << STATE_MARK_NON_FINAL;
        }

        QVector<Transition*> *transitions = state->transitions();
        qint64 num_transtitions           = transitions->size();

        out_stream << num_transtitions;
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

    out_file.close();
    return true;
}

bool TransducerManager::load(const QString &fname)
{
    clear_err_str();

    QFile in_file(fname);
    if (!in_file.open(QIODevice::ReadOnly)) {
        return set_err_str("Unable to open input file for reading");
    }

    QDataStream in_stream(&in_file);

    qint32 format_marker  = 0;
    qint32 format_version = 0;
    qint64 init_state_id  = 0;
    qint64 num_states     = 0;

    in_stream
        >> format_marker
        >> format_version // Currently ignored
        >> init_state_id
        >> num_states
    ;

    if (format_marker != TRANSDUCER_FORMAT_MARKER) {
        return set_err_str("Bad file format");
    }

    if (init_state_id == 0) {
        return set_err_str("Bad init_state_id");
    }

    if (num_states == 0) {
        return set_err_str("Bad number of states");
    }

    t->clear();

    QHash<uint, State*> *states  = t->states;
    QHash<qint64, State*> id2addr;
    while (!in_stream.atEnd()) {
        qint64 state_id   = 0;
        qint8  state_mark = 0;

        in_stream >> state_id;
        if (state_id == 0) {
            return set_err_str("Bad state_id");
        }

        State *state = TransducerManager::get_or_alloc_state(state_id, &id2addr);

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
        } else {
            return set_err_str("Bad state mark");
        }

        qint64 num_transitions = 0;
        in_stream >> num_transitions;
        if (num_transitions == 0 && !state->isFinal()) {
            return set_err_str("Bad state: 0 transitions for non-final state"); // FIXME: correct exit
        }

        for (int i = 0; i < num_transitions; i++) {
            QChar   label   = '\0';
            QString output  = "";
            qint64  next_id = 0;
            in_stream
                >> label
                >> output
                >> next_id
            ;

            if (next_id == 0) {
                return set_err_str("Bad next_id"); // FIXME: correct exit
            }
            if (label == '\0') {
                return set_err_str("Bad label"); // FIXME: correct exit
            }

            State *next = TransducerManager::get_or_alloc_state(next_id, &id2addr);

            state->setNext(label, next);
            state->setOutput(label, output);
        }

        states->insert(state->key(), state);
    }

    State *init_state = id2addr.value(init_state_id, NULL);
    if (init_state == NULL) {
        return set_err_str("Unknown init_state_id");
    }

    // TESTME: Test initial state only transducers

    t->init_state = init_state;

    in_file.close();

    return true;
}

/*static*/ QVector<State*>* TransducerManager::_initialize_tmp_states(int n)
{
    QVector<State*> *tmp_states = new QVector<State*>(n);
    for (int i = 0; i < n; i++) {
        (*tmp_states)[i] = new State();
    }
    return tmp_states;
}

/*static*/ void TransducerManager::_destroy_tmp_states(QVector<State*> *tmp_states)
{
    for (int i = 0; i < tmp_states->size(); i++) {
        delete tmp_states->at(i);
    }
    tmp_states->clear();
}

/*static*/ int TransducerManager::common_prefix_length(const QString &s1, const QString &s2)
{
    int prefix_len = 0;
    while (prefix_len < s1.length()
        && prefix_len < s2.length()
        && s1.at(prefix_len) == s2.at(prefix_len)
    ) {
        prefix_len++;
    }
    return prefix_len;
}

/*static*/ QString TransducerManager::common_prefix(const QString &s1, const QString &s2)
{
    return s1.left(common_prefix_length(s1, s2));
}

/*static*/ State* TransducerManager::get_or_alloc_state(qint64 state_id, QHash<qint64, State *> *id2addr)
{
    State *state = id2addr->value(state_id, NULL);
    if (state != NULL) {
        return state;
    }

    State *new_state = new State();
    id2addr->insert(state_id, new_state);
    return new_state;
}
