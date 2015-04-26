#include "master.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QPluginLoader loader("test-console-threads-plugin");

    if (!loader.load()) {
        qDebug() << "plugin not loaded";
        return 1;
    }

    qDebug() << "plugin loaded";
    QObject *plugin               = loader.instance();
    WorkerFactory *worker_factory = qobject_cast<WorkerFactory *>(plugin);
    if (!worker_factory) {
        qDebug() << "unable to instantiate worker_factory";
        return 1;
    }

    qDebug() << "worker_factory instantiated";
    QHash<QString, QString> params;
    worker_factory->init(params);

    QThread *master_thread = new QThread(&app);
    Master  *master        = new Master(worker_factory, 4);
    master->moveToThread(master_thread);

    QObject::connect(master_thread, SIGNAL(started()), master, SLOT(start()));
    QObject::connect(master_thread, SIGNAL(finished()), master, SLOT(deleteLater()));

    master_thread->start();
    while (!master->isFinished()) {
        QThread::yieldCurrentThread();
    }
    master_thread->quit();
    master_thread->wait();

    qDebug() << "====================";

    worker_factory->destroy();

    app.exit();
}
