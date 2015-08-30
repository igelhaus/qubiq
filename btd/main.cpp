#include "main.h"

const char *DEFAULT_IN_FNAME  = "src-transducer";
const char *DEFAULT_OUT_FNAME = "src-transducer-qutd";

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
        this, SLOT  (buildStatusUpdate(qint64, qint64))
    );
    QObject::connect(
        _tm , SIGNAL(buildFinished(bool, QString)),
        this, SLOT  (buildFinished(bool, QString))
    );

    build_thread->start();
}

void TransducerBuilder::buildStatusUpdate(qint64 bytes_read, qint64 bytes_total)
{
    float read_pct = (float)100.0 * (float)bytes_read / (float)bytes_total;
    std::cout << "Read: " << read_pct << "% (" << bytes_read << " / " << bytes_total << ")" << '\r';
}

void TransducerBuilder::buildFinished(bool status, QString message)
{
    t_manager = NULL;
    build_thread->quit();
    build_thread->wait();

    std::cout << "Read 100%                              " << std::endl;

    if (!status) {
        std::cout << "ERROR building: " << message.toUtf8().data() << std::endl;
        emit allDone();
        return;
    }

    std::cout << "Successfully built" << std::endl;

    if (run_selftest) {
        if (selfTest()) {
            std::cout << "Successfully self-tested" << std::endl;
        } else {
            std::cout << "WARNING: self-testing failed" << std::endl;
        }
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
        this, SLOT  (saveStatusUpdate(int, int))
    );
    QObject::connect(
        _tm,  SIGNAL(saveFinished(bool, QString)),
        this, SLOT  (saveFinished(bool, QString))
    );

    save_thread->start();
}

void TransducerBuilder::saveStatusUpdate(int states_saved, int states_total)
{
    std::cout << "Saved: " << states_saved << " / " << states_total << '\r';
}

void TransducerBuilder::saveFinished(bool status, QString message)
{
    t_manager = NULL;
    save_thread->quit();
    save_thread->wait();

    std::cout << "Saved 100%                              " << std::endl;

    if (status) {
        std::cout << "Successfully saved" << std::endl;
    } else {
        std::cout << "ERROR saving" << message.toUtf8().data() << std::endl;
    }

    emit allDone();
}

// FIXME: Make more sofisticated
bool TransducerBuilder::selfTest()
{
    QFile in_file(in_fname);

    if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    std::cout << "Started self-testing" << std::endl;

    int current_line  = 0;
    int num_not_found = 0;

    QString current_word;
    QString current_output;
    QTextStream in_stream(&in_file);
    while (!in_stream.atEnd()) {
        current_line++;

        QString line      = in_stream.readLine();
        QStringList parts = line.split("\t");
        current_word    = parts.at(0);
        current_output  = parts.at(1);

        TransducerSearchTrace trace;
        QStringList found = t->search(current_word, &trace);
        if (found.size() == 0) {
            QString formatted(current_word);
            formatted.insert(trace.reached_pos + 1, "<--");
            std::cout << "Not found on line: " << current_line << std::endl;
            std::cout << "word                 = " << formatted.toUtf8().data() << std::endl;
            std::cout << "is_transducer_ready  = " << (trace.is_transducer_ready ? "true" : "false") << std::endl;
            std::cout << "is_reached_pos_final = " << (trace.is_reached_pos_final? "true" : "false") << std::endl;
            std::cout << "reached_pos          = " << trace.reached_pos << std::endl;
            std::cout << "labels_at_failed     = " << trace.labels_at_failed.toUtf8().data() << std::endl;
            num_not_found++;
        }
    }
    in_file.close();

    std::cout << "Total not founds: " << num_not_found << std::endl;

    return num_not_found == 0;
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
        optInput   ("in" ,       "[STRING] Input file." , "in"),
        optOutput  ("out",       "[STRING] Output file.", "out"),
        optSelfTest("self-test", "[BOOLEAN] If specified, transducer is self-tested after building.")
    ;
    parser.addOption(optInput);
    parser.addOption(optOutput);
    parser.addOption(optSelfTest);

    parser.process(app);

    QString in_file (parser.isSet("in") ? parser.value("in")  : DEFAULT_IN_FNAME);
    QString out_file(parser.isSet("out")? parser.value("out") : DEFAULT_OUT_FNAME);

    TransducerBuilder builder(&app);
    builder.setInputFile(in_file);
    builder.setOutputFile(out_file);
    builder.setSelfTest(parser.isSet("self-test"));

    QObject::connect(&builder, SIGNAL(allDone()), &app, SLOT(quit()));
    builder.startBuilding();

    return app.exec();
}
