#ifndef MASTER_H
#define MASTER_H

#include <QtCore>

#include "worker_interfaces.h"
#include "worker.h"

class Master : public QObject {
    Q_OBJECT

private:
    WorkerFactory     *_worker_factory;
    QVector<QThread*> *_threads;

    int _num_workers;
    int _num_finished_workers;
public:
    Master(WorkerFactory *worker_factory, int num_workers);
    ~Master();

    bool isFinished() const;
    Q_SIGNAL void finished();

    Q_SLOT void workerReady(int worker_id, QVector<int>* data);
    Q_SLOT void start();
};

#endif // MASTER_H
