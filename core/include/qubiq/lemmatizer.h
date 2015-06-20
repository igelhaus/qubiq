#ifndef LEMMATIZER_H
#define LEMMATIZER_H

#include <QtCore>
#include <qubiq/lemmatizer_interfaces.h>

class Lemmatizer : public QObject {
    Q_OBJECT

public:
    virtual ~Lemmatizer() {}

    Q_SIGNAL void ready(int id, QHash<QString, InterimLexeme*> *result);

public slots:
    virtual void start() = 0;
};

#endif // LEMMATIZER_H
