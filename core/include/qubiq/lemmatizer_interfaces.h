#ifndef LEMMATIZER_INTERFACES_H
#define LEMMATIZER_INTERFACES_H

#include <QtCore>
#include <qubiq/text.h>
#include <qubiq/lemmatizer.h>

class LemmatizerFactory {
public:
    virtual ~LemmatizerFactory() {}

    virtual bool init(const QHash<QString, QString> &params) = 0;
    virtual bool destroy() = 0;

    virtual Lemmatizer* newLemmatizer(int id, const Text *text) = 0;
};

Q_DECLARE_INTERFACE(LemmatizerFactory, "org.qubiq.lemmatizerfactory/1.0")

#endif // LEMMATIZER_INTERFACES_H
