#ifndef LEMMATIZER_H
#define LEMMATIZER_H

#include <QtCore>
#include <qubiq/util/lexeme_index.h>

class Lemmatizer : public QObject {
    Q_OBJECT

public:
    virtual ~Lemmatizer() {}

public slots:
    virtual void start() = 0;

signals:
    void ready(int id, LexemeIndex *partial_index);
};

#endif // LEMMATIZER_H
