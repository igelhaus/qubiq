#include <QtCore>
#include "transducer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Transducer *transducer = new Transducer();

    transducer->build("lexemes.txt", 10);

    QStringList r1 = transducer->search("lexeme");
    qDebug() << "num_results =" << r1.size();

    QStringList r2 = transducer->search("lexemes");
    qDebug() << "num_results =" << r2.size();

    QStringList r3 = transducer->search("lex");
    qDebug() << "num_results =" << r3.size();

    delete transducer;

    app.exit();
}
