#ifndef WORKER_INTERFACES_H
#define WORKER_INTERFACES_H

#include <QtCore>

class Worker;

class WorkerFactory {
public:
    virtual ~WorkerFactory() {}

    virtual bool init(const QHash<QString, QString> &params) = 0;
    virtual bool destroy() = 0;

    virtual Worker* newWorker(int worker_id) = 0;
};

Q_DECLARE_INTERFACE(WorkerFactory, "org.qubiq.workerfactory/1.0")

#endif // WORKER_INTERFACES_H
