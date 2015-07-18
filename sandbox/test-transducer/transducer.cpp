#include "transducer.h"

Transducer::Transducer()
{
    states     = new QHash<QString, State*>();
    tmp_states = new QVector<State*>();
}

Transducer::~Transducer()
{
    delete states; // delete values
    for (int i = 0; i < tmp_states->size(); i++)
        delete tmp_states->at(i);
    delete tmp_states;
}

bool Transducer::build(const QString &fname, int max_word_size)
{
    QFile in_file(fname);
    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    if (max_word_size < 1) {
        max_word_size = 1024;
    }

    tmp_states->resize(0);
    for (int i = 0; i < max_word_size; i++) {
        tmp_states->append(new State());
    }
    // 0-w-0-o-0-r-0-d-0
    // 0-w-0-o-0-r-0-m-0
    // previous_len
    // current_len
    // prefix_len

    QString     current_word;
    QString     current_output;
    QString     previous_word("");
    QTextStream in_stream(&in_file);
    while (!in_stream.atEnd()) {
        QString line      = in_stream.readLine();
        QStringList parts = line.split("\t");
        current_word   = parts.at(0);
        current_output = parts.at(1);

        int prefix_len = common_prefix_length(previous_word, current_word);

        // We minimize the states from the suffix of the previous word
        for (int i = previous_word.length() /*= last previous state index*/; i >= prefix_len + 1; i--) {
            tmp_states->at(i - 1)->setNext(
                previous_word.at(i - 1),
                findEquivalent(tmp_states->at(i))
            );
        }
        // This loop intializes the tail states for the current word
        for (int i = prefix_len + 1; i <= current_word.length() /*= last current state index*/; i++) {
            tmp_states->at(i)->clear();
            tmp_states->at(i - 1)->setNext(
                current_word.at(i - 1),
                tmp_states->at(i)
            );
        }
        if (previous_word != current_word) {
            tmp_states->at(current_word.length())->setFinal(true);
            // FIXME: Do we need to mark outputs of the final state somehow?
        }
        // Optimize output:
        for (int i = 0; i < prefix_len; i++) {
            QString _output       = tmp_states->at(i)->output(current_word.at(i)); // FIXME: ref?
            QString output_prefix = common_prefix(_output, current_output);
            QString output_suffix = _output.right(_output.length() - output_prefix.length());

            tmp_states->at(i)->setOutput(current_word.at(i), output_prefix);

            QVector<Transition*> *t = tmp_states->at(i)->transitions();
            for (int j = 0; j < t->size(); j++) {
                t->at(j)->prependOutput(output_suffix);
            }
            if (tmp_states->at(i)->isFinal()) {
                tmp_states->at(i)->updateFinalsWithPrefix(output_suffix);
            }
            current_output = current_output.right(current_output.length() - output_prefix.length());
        }

        if (current_word == previous_word) {
            tmp_states->at(current_word.length())->addFinal(current_output);
        } else {
            tmp_states->at(prefix_len)->setOutput(current_word.at(prefix_len), current_output);
        }

        previous_word = current_word;
    }
    in_file.close();

    // Minimize last word
    for (int i = current_word.length() /*= last previous state index*/; i >= 1; i--) {
        tmp_states->at(i - 1)->setNext(
            previous_word.at(i - 1),
            findEquivalent(tmp_states->at(i))
        );
    }
    init_state = findEquivalent(tmp_states->at(0));

    return true;
}

State* Transducer::findEquivalent(const State *state)
{
    QString state_key = state->key();
    if (states->contains(state_key)) {
        return states->value(state_key);
    }
    State *_state = new State(*state);
    states->insert(state_key, _state);
    return _state;
}

int Transducer::common_prefix_length(const QString &s1, const QString &s2) const
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

QString Transducer::common_prefix(const QString &s1, const QString &s2) const
{
    return s1.left(common_prefix_length(s1, s2));
}
