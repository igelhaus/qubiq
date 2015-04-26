#include "myworkerfactory.h"

MyWorkerFactory::MyWorkerFactory()
{
    qDebug() << "MyWorkerFactory()";
}

MyWorkerFactory::~MyWorkerFactory()
{
    qDebug() << "~MyWorkerFactory()";
}

bool MyWorkerFactory::init(const QHash<QString, QString> &params)
{
    Q_UNUSED(params);
    return true;
}

bool MyWorkerFactory::destroy()
{
    return true;
}

Worker* MyWorkerFactory::newWorker(int worker_id)
{
    MyWorker* worker = new MyWorker(worker_id);
    return worker;
}
