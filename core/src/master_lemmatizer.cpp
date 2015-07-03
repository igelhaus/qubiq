#include <qubiq/master_lemmatizer.h>

MasterLemmatizer::MasterLemmatizer(Text *text, LemmatizerFactory *lemmatizer_factory, int num_lemmatizers)
{
    _lemmatizer_factory       = lemmatizer_factory;
    _num_lemmatizers          = num_lemmatizers;
    _num_finished_lemmatizers = 0;

    _text    = text;
    _threads = new QVector<QThread*>();
}

MasterLemmatizer::~MasterLemmatizer() {
//    qDebug() << "~Master";
    delete _threads;
}

void MasterLemmatizer::start()
{
    // FIXME: Ensure re-entrance
    _num_finished_lemmatizers = 0;
    for (int i = 0; i < _num_lemmatizers; ++i) {
//        qDebug() << "Creating and starting lemmatizer " << i;

        // Worker thread created below is owned by the master object.
        // Lemmatizer can't have an owner before it's moved to the worker thread.
        QThread *thread        = new QThread(this);
        Lemmatizer *lemmatizer = _lemmatizer_factory->newLemmatizer(i, _text);
        lemmatizer->moveToThread(thread);

        _threads->append(thread);

        QObject::connect(thread, SIGNAL(started()),  lemmatizer, SLOT(start()));
        QObject::connect(thread, SIGNAL(finished()), lemmatizer, SLOT(deleteLater()));
        QObject::connect(
            lemmatizer, SIGNAL(ready          (int, QHash<QString, InterimLexeme*> *)),
            this      , SLOT  (lemmatizerReady(int, QHash<QString, InterimLexeme*> *))
        );
        thread->start();
    }
}

void MasterLemmatizer::lemmatizerReady(int id, LexemeIndex *partial_index)
{
    Q_UNUSED(partial_index)
//    qDebug() << "From lemmatizer " << id << " result_length = " << result->keys().length();

    // FIXME: Process result: merge partial index into text

    _threads->at(id)->quit();
    _threads->at(id)->wait();
    _num_finished_lemmatizers++;
    if (_num_finished_lemmatizers == _num_lemmatizers) {
        _threads->resize(0);
        emit finished();
    }
}

bool MasterLemmatizer::isFinished() const
{
    return _num_finished_lemmatizers == _num_lemmatizers;
}
