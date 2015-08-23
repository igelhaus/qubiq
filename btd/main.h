#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>

#include <QtCore>
#include <QCoreApplication>

#include <qubiq/util/transducer.h>
#include <qubiq/util/transducer_manager.h>

class TransducerBuilder : public QObject {
    Q_OBJECT

public:
    TransducerBuilder(QObject *parent = NULL);
    ~TransducerBuilder();

    inline void setInputFile (const QString &fname) { in_fname = fname; }
    inline void setOutputFile(const QString &fname) { out_fname = fname; }

    void startBuilding();
    bool selfTest();
    void startSaving();

public slots:
    void buildProgress(qint64 bytes_read, qint64 bytes_total);
    void buildFinished(bool status);

    void saveProgress(int states_saved, int states_total);
    void saveFinished(bool status);

signals:
    void allDone();

private:

    Transducer        *t;
    TransducerManager *t_manager;

    QString in_fname;
    QString out_fname;
    bool    run_selftest;

    QThread *build_thread;
    QThread *save_thread;

private slots:
    inline void _start_building() { t_manager->build(in_fname); }
    inline void _start_saving()   { t_manager->save(out_fname); }
};

#endif // _MAIN_H_
