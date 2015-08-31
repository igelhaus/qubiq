#include <qubiq/util/transducer_manager.h>

/**
 * \class TransducerManager
 *
 * \brief The TransducerManager class provides means for working with transducers, i.e. building
 * new transducers from the source lists and serializing into / deserializing from the binary format.
 *
 * \sa Transducer
 */

const int DEFAULT_MAX_WORD_SIZE = 1024;

const qint8 STATE_MARK_FINAL     = 'f';
const qint8 STATE_MARK_NON_FINAL = 'F';

const qint32 TRANSDUCER_FORMAT_MARKER  = 0x51555444; // QUTD = Qubiq Util TransDucer
const qint32 TRANSDUCER_FORMAT_VERSION = 1;

const int DEFAULT_SAVE_LOAD_STATUS_UPDATE_STEP = 1024; // Report status after each X states are saved/loaded
const qint64 DEFAULT_BUILD_STATUS_UPDATE_STEP  = 4096; // Report status after approximately X bytes read
const qint64 DEFAULT_BUILD_STATUS_UPDATE_LOWER =   50;
const qint64 DEFAULT_BUILD_STATUS_UPDATE_UPPER = 4046;

/**
 * \brief Constructs a TransducerManager object implicitly creating a new \c Transducer.
 * \param[in] parent Parent object.
 */
TransducerManager::TransducerManager(QObject *parent) : QObject(parent)
{
    t             = new Transducer();
    is_self_alloc = true;
}

/**
 * \brief This is an overloaded constructor. Constructs a TransducerManager object for managing already created \c Transducer.
 * \param[in] transducer Transducer to be managed by the current instance of \c TransducerManager.
 * \param[in] parent     Parent object.
 */
//!
TransducerManager::TransducerManager(Transducer *transducer, QObject *parent) : QObject(parent)
{
    t             = transducer;
    is_self_alloc = false;
}

//! Destructs the TransducerManager object.
TransducerManager::~TransducerManager()
{
    if (is_self_alloc)
        delete t;
}

/*
 * Example state-transition chains:
 *
 * 0-w-0-o-0-r-0-d-0
 * 0-w-0-o-0-r-0-m-0
 *
 */
/**
 * \brief Builds a transducer in-memory from the source file.
 *
 * The source file for building a trsnducer should be a text file where each line should have format
 *
 * token<tab character>data associated with the token
 *
 * See tests for an example.
 *
 * \param[in] fname         Source file name.
 * \param[in] max_word_size Maximum token size. 0 or negative value falls back to \c DEFAULT_MAX_WORD_SIZE.
 *
 * \return \c true if build is successful, \c false otherwise. In case of errors, error message is set.
 */
bool TransducerManager::build(const QString &fname, int max_word_size /*= 0*/)
{
    clear_err_str();
    t->clear();

    QFile in_file(fname);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        set_err_str("Unable to open input file for reading");
        emit buildFinished(false, err_str);
        return false;
    }

    const qint64 num_bytes = in_file.size();
    qint64 num_bytes_read  = 0;

    if (num_bytes == 0) {
        set_err_str("Input file is of zero length");
        emit buildFinished(false, err_str);
        return false;
    }

    if (max_word_size < 1) {
        max_word_size = DEFAULT_MAX_WORD_SIZE;
    }

    QVector<State*> *tmp_states = TransducerManager::_initialize_tmp_states(max_word_size + 1);

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

        if (current_len > max_word_size) {
            TransducerManager::_destroy_tmp_states(tmp_states);
            set_err_str("Length of current word is more than max_word_size");
            emit buildFinished(false, err_str);
            return false;
        }

        int prefix_len = TransducerManager::common_prefix_length(previous_word, current_word);

        // We minimize the states from the suffix of the previous word
        for (int i = previous_word.length() /*= last previous state index*/; i >= prefix_len + 1; i--) {
            tmp_states->at(i - 1)->setNext(
                previous_word.at(i - 1), get_or_alloc_state(tmp_states->at(i))
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
        }
        // Optimize output:
        for (int i = 0; i < prefix_len; i++) {
            QString _output       = tmp_states->at(i)->output(current_word.at(i));
            QString output_prefix = TransducerManager::common_prefix(_output, current_output);
            QString output_suffix = _output.right(_output.length() - output_prefix.length());

            tmp_states->at(i)->setOutput(current_word.at(i), output_prefix);
            tmp_states->at(i)->updateOutputsWithPrefix(output_suffix);
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

        // Report status:
        num_bytes_read += (qint64)(line.length()) * 2; // NB! Approximate number of bytes
        const qint64 remainder = num_bytes_read % DEFAULT_BUILD_STATUS_UPDATE_STEP;
        if (remainder < DEFAULT_BUILD_STATUS_UPDATE_LOWER || remainder > DEFAULT_BUILD_STATUS_UPDATE_UPPER) {
            emit buildStatusUpdate(num_bytes_read, num_bytes);
        }
    }
    in_file.close();

    // Minimize the last word
    for (int i = current_word.length() /*= last previous state index*/; i >= 1; i--) {
        tmp_states->at(i - 1)->setNext(
            current_word.at(i - 1), get_or_alloc_state(tmp_states->at(i))
        );
    }

    t->init_state = get_or_alloc_state(tmp_states->at(0));

    TransducerManager::_destroy_tmp_states(tmp_states);

    emit buildFinished(true, err_str);
    return true;
}

/**
 * \brief Serializes transducer on disk in a binary QUTD format.
 *
 * BNF for the current version of the QUTD format is:
 *
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
 *
 * \param[in] fname File name to write to.
 *
 * \return \c true if the transducer was serialized, \c false otherwise. In case of errors, error message is set.
 *
 * \sa load
 */
bool TransducerManager::save(const QString &fname)
{
    clear_err_str();

    if (!t->isReady()) {
        set_err_str("Unable to save unready transducer");
        emit saveFinished(false, err_str);
        return false;
    }

    QFile out_file(fname);
    if (!out_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        set_err_str("Unable to open output file for writing");
        emit saveFinished(false, err_str);
        return false;
    }

    QDataStream out_stream(&out_file);

    QSet<State> *states = t->states;

    out_stream
        << TRANSDUCER_FORMAT_MARKER
        << TRANSDUCER_FORMAT_VERSION
        << (qint64)(t->init_state)
        << (qint64)(states->size())
    ;

    const int num_states = states->size();
    int num_states_saved = 0;

    QSet<State>::const_iterator i = states->constBegin();
    while (i != states->constEnd()) {
        const State *state = i.operator ->();
        out_stream << (qint64)state;

        if (state->isFinal()) {
            out_stream
                << STATE_MARK_FINAL
                << *(state->finalStrings())
            ;
        } else {
            out_stream << STATE_MARK_NON_FINAL;
        }

        QHash<QChar, Transition*> *transitions = state->transitions();
        qint64 num_transtitions                = transitions->size();

        out_stream << num_transtitions;
        QHash<QChar, Transition*>::iterator i_t;
        for (i_t = transitions->begin(); i_t != transitions->end(); ++i_t) {
            Transition *transition = i_t.value();
            State *next = transition->next();
            out_stream
                << transition->label()
                << transition->output()
                << (qint64)next
            ;
        }

        if ((++num_states_saved) % DEFAULT_SAVE_LOAD_STATUS_UPDATE_STEP == 0) {
            emit saveStatusUpdate(num_states_saved, num_states);
        }
        ++i;
    }

    out_file.close();

    emit saveFinished(true, err_str);
    return true;
}

/**
 * \brief Reads a transducer from the QUTD file into memory.
 *
 * \param[in] fname File name to read from.
 *
 * \return \c true if the transducer was read, \c false otherwise. In case of errors, error message is set.
 *
 * \sa save
 */
bool TransducerManager::load(const QString &fname)
{
    clear_err_str();

    QFile in_file(fname);
    if (!in_file.open(QIODevice::ReadOnly)) {
        set_err_str("Unable to open input file for reading");
        emit loadFinished(false, err_str);
        return false;
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

    bool is_prologue_ok = true;
    if (format_marker != TRANSDUCER_FORMAT_MARKER) {
        is_prologue_ok = set_err_str("Bad file format");
    } else if (init_state_id == 0) {
        is_prologue_ok = set_err_str("Bad init_state_id");
    } else if (num_states == 0) {
        is_prologue_ok = set_err_str("Bad number of states");
    }

    if (!is_prologue_ok) {
        emit loadFinished(is_prologue_ok, err_str);
        return is_prologue_ok;
    }

    int num_states_read = 0;

    QHash<qint64, State*> id2addr;
    while (!in_stream.atEnd()) {
        if (num_states_read > num_states) {
            set_err_str("Read more states than declared");
            emit loadFinished(false, err_str);
            return false;
        }

        qint64 state_id   = 0;
        qint8  state_mark = 0;

        in_stream >> state_id;
        if (state_id == 0) {
            set_err_str("Bad state_id");
            emit loadFinished(false, err_str);
            return false;
        }

        State *state = get_or_alloc_state(state_id, &id2addr);

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
            set_err_str("Bad state mark");
            emit loadFinished(false, err_str);
            return false;
        }

        qint64 num_transitions = 0;
        in_stream >> num_transitions;
        if (num_transitions == 0 && !state->isFinal()) {
            set_err_str("Bad state: 0 transitions for non-final state");
            emit loadFinished(false, err_str);
            return false;
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
                set_err_str("Bad next_id");
                emit loadFinished(false, err_str);
                return false;
            }
            if (label == '\0') {
                set_err_str("Bad label");
                emit loadFinished(false, err_str);
                return false;
            }

            State *next = get_or_alloc_state(next_id, &id2addr);

            state->setNext(label, next);
            state->setOutput(label, output);
        }

        if ((++num_states_read) % DEFAULT_SAVE_LOAD_STATUS_UPDATE_STEP == 0) {
            emit loadStatusUpdate(num_states_read, num_states);
        }
    }

    t->clear();
    State *init_state = NULL;

    QHash<qint64, State*>::iterator i;
    for (i = id2addr.begin(); i != id2addr.end(); ++i) {
        State *state = i.value();
        QSet<State>::iterator inserted = t->states->insert(*state);
        if (i.key() == init_state_id) {
            init_state = const_cast<State*>(inserted.operator ->());
        }
        delete state;
    }

    if (init_state == NULL) {
        set_err_str("Unknown init_state_id");
        emit loadFinished(false, err_str);
        return false;
    }

    t->init_state = init_state;

    in_file.close();

    emit loadFinished(true, err_str);
    return true;
}

//! \internal Initializes a vector of temporary states for building a transducer from the source file.
/*static*/ QVector<State*>* TransducerManager::_initialize_tmp_states(int n)
{
    QVector<State*> *tmp_states = new QVector<State*>(n);
    for (int i = 0; i < n; i++) {
        (*tmp_states)[i] = new State();
    }
    return tmp_states;
}

//! \internal Destroys the vector of temporary states for building a transducer from the source file.
/*static*/ void TransducerManager::_destroy_tmp_states(QVector<State*> *tmp_states)
{
    qDeleteAll(tmp_states->begin(), tmp_states->end());
    delete tmp_states;
}

//! \internal Calculates common prefix length between strings \c s1 and \c s2.
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

//! \internal Finds common prefix length between strings \c s1 and \c s2.
/*static*/ QString TransducerManager::common_prefix(const QString &s1, const QString &s2)
{
    return s1.left(TransducerManager::common_prefix_length(s1, s2));
}

//! \internal Finds a state among already constructed or allocates a new one (used on \c build).
State* TransducerManager::get_or_alloc_state(const State *state)
{
    QSet<State>::iterator found = t->states->insert(*state);
    return const_cast<State*>(found.operator ->());
}

//! \internal Finds a state among already constructed or allocates a new one (used on \c load).
State* TransducerManager::get_or_alloc_state(qint64 state_id, QHash<qint64, State*> *id2addr)
{
    State *state = id2addr->value(state_id, NULL);
    if (state != NULL) {
        return state;
    }

    state = new State();
    id2addr->insert(state_id, state);

    return state;
}
