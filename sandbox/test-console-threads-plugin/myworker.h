#ifndef MYWORKER_H
#define MYWORKER_H

#include "test-console-threads-plugin_global.h"
#include "../test-console-threads/worker.h"

class MyWorker : public Worker {
    Q_OBJECT

private:
    QNetworkAccessManager *_net;

    QVector<int> *_data;
    int          _id;
    Q_SLOT void process_data(QNetworkReply *reply);

public:
    MyWorker(int worker_id);
    ~MyWorker();

    Q_SLOT void start();
};

#endif // MYWORKER_H
