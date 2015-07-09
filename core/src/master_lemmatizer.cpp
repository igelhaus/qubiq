#include <qubiq/master_lemmatizer.h>

MasterLemmatizer::MasterLemmatizer(Text *text, LemmatizerFactory *lemmatizer_factory, int num_lemmatizers /*= 0*/)
{
    if (num_lemmatizers < 1) {
        int ideal_thread_count = QThread::idealThreadCount();
        num_lemmatizers        = ideal_thread_count > 1? ideal_thread_count - 1 : 1;
    }

    _lemmatizer_factory       = lemmatizer_factory;
    _num_lemmatizers          = num_lemmatizers;
    _num_finished_lemmatizers = 0;

    _text    = text;
    _threads = new QVector<QThread*>();
}

MasterLemmatizer::~MasterLemmatizer() {
    delete _threads;
}

bool MasterLemmatizer::start()
{
    if (!isFinished()) {
        LOG_WARNING() << "Master lemmatizer already running";
        return false;
    }
    _num_finished_lemmatizers = 0;
    for (int i = 0; i < _num_lemmatizers; ++i) {
        LOG_INFO() << "Creating and starting lemmatizer " << i;

        // Worker thread created below is owned by the master object.
        // Lemmatizer can't have an owner before it's moved to the worker thread.
        QThread *thread        = new QThread(this);
        Lemmatizer *lemmatizer = _lemmatizer_factory->newLemmatizer(i, _text);
        lemmatizer->moveToThread(thread);

        _threads->append(thread);

        QObject::connect(thread, SIGNAL(started()),  lemmatizer, SLOT(start()));
        QObject::connect(thread, SIGNAL(finished()), lemmatizer, SLOT(deleteLater()));
        QObject::connect(
            lemmatizer, SIGNAL(ready          (int, LexemeIndex*)),
            this      , SLOT  (lemmatizerReady(int, LexemeIndex*))
        );
        thread->start();
    }
    return true;
}

void MasterLemmatizer::lemmatizerReady(int id, LexemeIndex *partial_index)
{
    LOG_INFO() << "Got result from lemmatizer " << id;
    LOG_INFO() << "partial_index_size = " << partial_index->size();

    // FIXME: partial_index == NULL as an indicator of lemmatizer's failure?

    _text->lexemes()->merge(*partial_index);
    delete partial_index;

    _threads->at(id)->quit();
    _threads->at(id)->wait();
    _num_finished_lemmatizers++;

    LOG_INFO() << "Number of already finished lemmatizers: " << _num_finished_lemmatizers;

    if (_num_finished_lemmatizers == _num_lemmatizers) {
        LOG_DEBUG() << "Emitting 'finished' signal";
        _threads->resize(0);
        emit finished();
    }
}
