#ifndef _MASTER_LEMMATIZER_H_
#define _MASTER_LEMMATIZER_H_

#include <QtCore>

#include <qubiq/util/lexeme_index.h>
#include <qubiq/text.h>
#include <qubiq/lemmatizer_interfaces.h>
#include <qubiq/lemmatizer.h>

class MasterLemmatizer : public QObject {
    Q_OBJECT

private:
    LemmatizerFactory *_lemmatizer_factory;
    QVector<QThread*> *_threads;

    Text *_text;

    int _num_lemmatizers;
    int _num_finished_lemmatizers;
public:
    MasterLemmatizer(Text *text, LemmatizerFactory *lemmatizer_factory, int num_lemmatizers);
    ~MasterLemmatizer();

    bool isFinished() const;
    Q_SIGNAL void finished();

    Q_SLOT void lemmatizerReady(int id, LexemeIndex *partial_index);
    Q_SLOT void start();
};

#endif // _MASTER_LEMMATIZER_H_
