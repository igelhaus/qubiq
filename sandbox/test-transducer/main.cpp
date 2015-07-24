#include <QtCore>
#include "transducer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Transducer *transducer = new Transducer();

    transducer->build("lexemes.txt", 10);

    QStringList r = transducer->search("lexeme");
    qDebug() << "num_results =" << r.size();

    delete transducer;

    app.exit();
}
