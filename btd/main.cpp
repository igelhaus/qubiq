#include "main.h"

TransducerBuilder::TransducerBuilder(QObject *parent) : QObject(parent)
{
    t            = new Transducer();
    t_manager    = NULL;

    build_thread = new QThread();
    save_thread  = new QThread();

    run_selftest = false;
}

TransducerBuilder::~TransducerBuilder()
{
    delete t;
    delete build_thread;
    delete save_thread;
}

void TransducerBuilder::startBuilding()
{
    TransducerManager *_tm = new TransducerManager(t);
    _tm->moveToThread(build_thread);
    t_manager = _tm;

    QObject::connect(build_thread, SIGNAL(started()) , this, SLOT(_start_building()));
    QObject::connect(build_thread, SIGNAL(finished()),  _tm, SLOT(deleteLater()));
    QObject::connect(
        _tm , SIGNAL(buildStatusUpdate(qint64, qint64)),
        this, SLOT  (buildProgress    (qint64, qint64))
    );
    QObject::connect(
        _tm , SIGNAL(buildFinished(bool, QString)),
        this, SLOT  (buildFinished(bool, QString))
    );

    build_thread->start();
}

void TransducerBuilder::buildProgress(qint64 bytes_read, qint64 bytes_total)
{
    std::cout << "Bytes read: " << bytes_read << " / " << bytes_total << '\r';
}

void TransducerBuilder::buildFinished(bool status, QString message)
{
    t_manager = NULL;
    build_thread->quit();
    build_thread->wait();

    std::cout << std::endl;

    if (!status) {
        std::cout << "ERROR building: " << message.toUtf8().data() << std::endl;
        emit allDone();
        return;
    }

    std::cout << "Successfully built" << std::endl;

    if (run_selftest && !selfTest()) {
        std::cout << "ERROR self-testing" << std::endl;
        emit allDone();
        return;
    }

    startSaving();
}

void TransducerBuilder::startSaving()
{
    TransducerManager *_tm = new TransducerManager(t);
    _tm->moveToThread(save_thread);
    t_manager = _tm;

    QObject::connect(save_thread, SIGNAL(started()) , this, SLOT(_start_saving()));
    QObject::connect(save_thread, SIGNAL(finished()),  _tm, SLOT(deleteLater()));
    QObject::connect(
        _tm,  SIGNAL(saveStatusUpdate(int, int)),
        this, SLOT  (saveProgress    (int, int))
    );
    QObject::connect(
        _tm,  SIGNAL(saveFinished(bool, QString)),
        this, SLOT  (saveFinished(bool, QString))
    );

    save_thread->start();
}

void TransducerBuilder::saveProgress(int states_saved, int states_total)
{
    std::cout << "Saved: " << states_saved << " / " << states_total << '\r';
}

void TransducerBuilder::saveFinished(bool status, QString message)
{
    t_manager = NULL;
    save_thread->quit();
    save_thread->wait();

    std::cout << std::endl;

    if (status) {
        std::cout << "Successfully saved" << std::endl;
    } else {
        std::cout << "ERROR saving" << message.toUtf8().data() << std::endl;
    }

    emit allDone();
}

bool TransducerBuilder::selfTest()
{
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("build-transducer");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "build-transducer: Simple command-line utility"
        " for compiling QUTD transducers using libqubiqutil"
    );
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption
        optInput ("in" , "[STRING] Input file." , "in"),
        optOutput("out", "[STRING] Output file.", "out")
    ;
    parser.addOption(optInput);
    parser.addOption(optOutput);

    parser.process(app);

    TransducerBuilder builder(&app);
    builder.setInputFile(parser.value("in"));
    builder.setOutputFile(parser.value("out"));

    QObject::connect(&builder, SIGNAL(allDone()), &app, SLOT(quit()));
    builder.startBuilding();

    return app.exec();
}
