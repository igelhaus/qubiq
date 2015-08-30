#ifndef _TRANSDUCER_MANAGER_H_
#define _TRANSDUCER_MANAGER_H_

#include <QtCore>
#include <QtAlgorithms>
#include <qubiq/util/transducer.h>

class TransducerManager : public QObject
{
    Q_OBJECT

public:
    TransducerManager(QObject *parent = 0);
    TransducerManager(Transducer *transducer, QObject *parent = 0);
    ~TransducerManager();

    inline const Transducer *transducer() const { return t; }

    inline QString error() const { return err_str; }

public slots:
    bool build(const QString &fname, int max_word_size = 0);
    bool save(const QString &fname);
    bool load(const QString &fname);

signals:

    /**
     * \brief Signal emitted when a new chunk of input is processed during build.
     * \param[in] bytes_read  Bytes read so far.
     * \param[in] bytes_total Total size of input in bytes.
     */
    void buildStatusUpdate(qint64 bytes_read, qint64 bytes_total);

    /**
     * \brief Signal emitted when a new portion of states is processed during serialization.
     * \param[in] states_saved Number of states serialized so far.
     * \param[in] states_total Total number of states in the transducer associated with \c this manager.
     */
    void saveStatusUpdate(int states_saved, int states_total);

    /**
     * \brief Signal emitted when a new portion of states is processed during deserialization.
     * \param[in] states_read  Number of states deserialized so far.
     * \param[in] states_saved Total number of states expected to be read into the transducer associated with \c this manager.
     */
    void loadStatusUpdate(int states_read, int states_total);

    /**
     * \brief Signal emitted when build is complete (either successfully or not).
     * \param[in] status  \c true on success, \c false on failure.
     * \param[in] message Error message or an empty string if build succeeded.
     */
    void buildFinished(bool status, QString message);

    /**
     * \brief Signal emitted when serialization is complete (either successfully or not).
     * \param[in] status  \c true on success, \c false on failure.
     * \param[in] message Error message or an empty string if serialization succeeded.
     */
    void saveFinished(bool status, QString message);

    /**
     * \brief Signal emitted when deserialization is complete (either successfully or not).
     * \param[in] status  \c true on success, \c false on failure.
     * \param[in] message Error message or an empty string if deserialization succeeded.
     */
    void loadFinished(bool status, QString message);

private:
    friend class TestTransducer;

    Transducer *t;            //!< Pointer to the transducer being managed by \c this manager.
    bool       is_self_alloc; //!< Flag indicating whether \c this manager allocated the managed transducer.
    QString    err_str;       //!< Buffer holding error message (if any) from the last operation.

    //! \internal Sets error message.
    inline bool set_err_str(const char *msg) { err_str = msg; return false; }

    //! \internal Clears error message.
    inline void clear_err_str() { err_str.clear(); }

    State* get_or_alloc_state(const State *state);
    State* get_or_alloc_state(qint64 state_id, QHash<qint64, State*> *id2addr);

    static QVector<State*>* _initialize_tmp_states(int n);
    static void _destroy_tmp_states(QVector<State*> *tmp_states);

    static int common_prefix_length(const QString &s1, const QString &s2);
    static QString common_prefix(const QString &s1, const QString &s2);
};

#endif // _TRANSDUCER_MANAGER_H_
