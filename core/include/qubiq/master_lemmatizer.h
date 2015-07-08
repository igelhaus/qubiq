#ifndef _MASTER_LEMMATIZER_H_
#define _MASTER_LEMMATIZER_H_

#include <QtCore>
#include <cutelogger/include/Logger.h>
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
    MasterLemmatizer(Text *text, LemmatizerFactory *lemmatizer_factory, int num_lemmatizers = -1);
    ~MasterLemmatizer();

    inline bool isFinished() const { return _num_finished_lemmatizers == _num_lemmatizers; }

public slots:
    void lemmatizerReady(int id, LexemeIndex *partial_index);
    bool start();

signals:
    void finished();
};

#endif // _MASTER_LEMMATIZER_H_
