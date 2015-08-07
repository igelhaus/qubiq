#include <QtCore>
#include "transducer.h"
#include "transducer_manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    TransducerManager t_manager1;
    t_manager1.build("lexemes.txt", 10);

    qDebug() << "num_results =" << t_manager1.transducer()->search("lexeme") .size();
    qDebug() << "num_results =" << t_manager1.transducer()->search("lexemes").size();
    qDebug() << "num_results =" << t_manager1.transducer()->search("lex")    .size();

    t_manager1.save("lexemes.qutd");

    TransducerManager t_manager2;
    t_manager2.load("lexemes.qutd");
    qDebug() << "num_results =" << t_manager2.transducer()->search("lexeme") .size();
    qDebug() << "num_results =" << t_manager2.transducer()->search("lexemes").size();
    qDebug() << "num_results =" << t_manager2.transducer()->search("lex")    .size();

    qDebug() << "num_results =" << t_manager1.transducer()->search("lexemezzzz") .size();
    qDebug() << "num_results =" << t_manager2.transducer()->search("lexemezzzz") .size();

    app.exit();
}
