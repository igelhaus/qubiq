#ifndef LEMMATIZER_H
#define LEMMATIZER_H

#include <QtCore>
#include <qubiq/util/lexeme_index.h>
#include <qubiq/lemmatizer_interfaces.h>

class Lemmatizer : public QObject {
    Q_OBJECT

public:
    virtual ~Lemmatizer() {}

    Q_SIGNAL void ready(int id, LexemeIndex *partial_index);

public slots:
    virtual void start() = 0;
};

#endif // LEMMATIZER_H
