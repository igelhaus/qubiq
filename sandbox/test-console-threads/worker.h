#ifndef WORKER_H
#define WORKER_H

#include <QtCore>

class Worker : public QObject {
    Q_OBJECT

public:
    virtual ~Worker() {}

    Q_SIGNAL void dataReady(int worker_id, QVector<int>* data);

public slots:
    virtual void start() = 0;
};

#endif // WORKER_H
