#include <QtTest/QtTest>
#include <qubiq/util/transducer.h>
#include <qubiq/util/transducer_manager.h>

class TestTransducer: public QObject
{
    Q_OBJECT

private slots:
    void simpleTransducer();
};

void TestTransducer::simpleTransducer()
{
    // NB! This test depends on DEFAULT_READ_BUFFER_SIZE value
    QTemporaryFile transducer_source;
    transducer_source.open();
    transducer_source.write("lexeme\tlexeme\n");
    transducer_source.write("lexemes\tlexeme\n");
    transducer_source.close();

    TransducerManager t_manager1;
    t_manager1.build(transducer_source.fileName(), 10);

    QCOMPARE(t_manager1.transducer()->search("aaaaaa"       ).size(), 0);
    QCOMPARE(t_manager1.transducer()->search("lexeme"       ).size(), 1);
    QCOMPARE(t_manager1.transducer()->search("lexemes"      ).size(), 1);
    QCOMPARE(t_manager1.transducer()->search("lex"          ).size(), 0);
    QCOMPARE(t_manager1.transducer()->search("lexemezzz"    ).size(), 0);

    t_manager1.save("./lexemes.qutd");

    TransducerManager t_manager2;
    t_manager2.load("./lexemes.qutd");

    QCOMPARE(t_manager2.transducer()->search("aaaaaa"       ).size(), 0);
    QCOMPARE(t_manager2.transducer()->search("lexeme"       ).size(), 1);
    QCOMPARE(t_manager2.transducer()->search("lexemes"      ).size(), 1);
    QCOMPARE(t_manager2.transducer()->search("lex"          ).size(), 0);
    QCOMPARE(t_manager2.transducer()->search("lexemezzz"    ).size(), 0);
}

QTEST_MAIN(TestTransducer)
#include "test_transducer.moc"
