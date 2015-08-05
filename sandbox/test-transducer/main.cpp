#include <QtCore>
#include "transducer.h"
#include "transducer_manager.h"

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

    TransducerManager t_manager1(transducer);

    t_manager1.saveTofile("lexemes.qutd");

    TransducerManager t_manager2;
    t_manager2.loadFromFile("lexemes.qutd");
    qDebug() << "num_results =" << t_manager2.transducer()->search("lexeme") .size();
    qDebug() << "num_results =" << t_manager2.transducer()->search("lexemes").size(); // FIXME
    qDebug() << "num_results =" << t_manager2.transducer()->search("lex")    .size();

    delete transducer;

    app.exit();
}
