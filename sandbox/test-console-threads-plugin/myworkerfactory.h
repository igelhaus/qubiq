#ifndef MYWORKERFACTORY_H
#define MYWORKERFACTORY_H

#include "test-console-threads-plugin_global.h"
#include "myworker.h"

class Worker;

class MyWorkerFactory : public QObject, public WorkerFactory {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qubiq.workerfactory/1.0")
    Q_INTERFACES(WorkerFactory)

public:
    MyWorkerFactory();
    ~MyWorkerFactory();

    bool init(const QHash<QString, QString> &params);
    bool destroy();

    Worker* newWorker(int worker_id);
};

#endif // MYWORKERFACTORY_H
