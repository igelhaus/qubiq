#ifndef TRANSDUCER_MANAGER_H
#define TRANSDUCER_MANAGER_H

#include <QtCore>
#include <qubiq/util/transducer.h>

class TransducerManager : public QObject
{
    Q_OBJECT

public:
    TransducerManager(QObject *parent = 0);
    TransducerManager(Transducer *other, QObject *parent = 0);
    ~TransducerManager();

    inline const Transducer *transducer() const { return t; }

    inline QString error() const { return err_str; }

public slots:
    bool build(const QString &fname, int max_word_size = 0);
    bool save(const QString &fname);
    bool load(const QString &fname);

signals:
    void buildFinished();
    void saveFinished();
    void loadFinished();

private:
    bool is_self_alloc;
    Transducer *t;

    QString err_str;

    inline bool set_err_str(const char *msg) { err_str = msg; return false; }
    inline void clear_err_str() { err_str.clear(); }

    static QVector<State*>* _initialize_tmp_states(int n);
    static void _destroy_tmp_states(QVector<State*> *tmp_states);

    static int common_prefix_length(const QString &s1, const QString &s2);
    static QString common_prefix(const QString &s1, const QString &s2);
    static State *get_or_alloc_state(qint64 state_id, QHash<qint64, State*> *id2addr);
};

#endif // TRANSDUCER_MANAGER_H
