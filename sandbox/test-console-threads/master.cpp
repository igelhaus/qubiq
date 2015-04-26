#include "master.h"

Master::Master(WorkerFactory *worker_factory, int num_workers)
{
    _worker_factory       = worker_factory;
    _num_workers          = num_workers; // FIXME: add calculating default
    _num_finished_workers = 0;
    _threads              = new QVector<QThread*>();
}

Master::~Master() {
    qDebug() << "~Master";
    delete _threads;
}

void Master::start()
{
    _num_finished_workers = 0;
    for (int i = 0; i < _num_workers; ++i) {
        qDebug() << "Starting worker " << i;

        QThread *thread = new QThread(this);             // thread owned by the master object
        Worker *worker  = _worker_factory->newWorker(i); // can't have an owner before it's moved to another thread
        worker->moveToThread(thread);

        _threads->append(thread);

        QObject::connect(thread, SIGNAL(started()), worker, SLOT(start()));
        QObject::connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
        QObject::connect(worker, SIGNAL(dataReady(int, QVector<int>*)), this, SLOT(workerReady(int, QVector<int>*)));
        thread->start();
    }
}

void Master::workerReady(int worker_id, QVector<int>* data)
{
    qDebug() << "From worker " << worker_id << " data_length = " << data->length();
    _threads->at(worker_id)->quit();
    _threads->at(worker_id)->wait();
    _num_finished_workers++;
    if (_num_finished_workers == _num_workers) {
        _threads->resize(0);
        emit finished();
    }
}

bool Master::isFinished() const
{
    return _num_finished_workers == _num_workers;
}
