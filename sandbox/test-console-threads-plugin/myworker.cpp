#include "myworker.h"

MyWorker::MyWorker(int worker_id)
{
    _id   = worker_id;
    _net  = new QNetworkAccessManager(this);
    _data = new QVector<int>();

    QObject::connect(_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(process_data(QNetworkReply*)));
}

MyWorker::~MyWorker()
{
    qDebug() << "~MyWorker " << _id;
    delete _net;
    delete _data;
}

void MyWorker::start()
{
    qDebug() << "MyWorker started: " << _id;

    QNetworkRequest req(QUrl(QString("http://127.0.0.1/?token=%1").arg(_id)));
    _net->get(req);
}

void MyWorker::process_data(QNetworkReply *reply)
{
    qsrand(_id * QTime::currentTime().msec());
    int max_item = 2000000 + qrand() % 3000001;

    qDebug() << QString("Worker %1: received reply to %2 (size %3), now starting working on payload %4")
        .arg(_id)
        .arg(reply->request().url().toString())
        .arg(reply->bytesAvailable())
        .arg(max_item);

    for (int i = 0; i < max_item; i++) {
        _data->append(i);
        for (int j = 0; j < 10; j++) {
            QVector<int>* aux_data = new QVector<int>();
            delete aux_data;
        }
    }

    reply->deleteLater();
    emit dataReady(_id, _data);
}
