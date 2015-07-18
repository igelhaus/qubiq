#include <QtCore>
#include "transducer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Transducer *transducer = new Transducer();

    transducer->build("lexemes.txt", 10);

    delete transducer;

    app.exit();
}
