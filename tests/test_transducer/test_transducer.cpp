#include <QtTest/QtTest>
#include <qubiq/util/transducer_state.h>
#include <qubiq/util/transducer.h>
#include <qubiq/util/transducer_manager.h>

class TestTransducer: public QObject
{
    Q_OBJECT

private slots:
    void testTransducerState();
    void simpleTransducer();
};

void TestTransducer::testTransducerState()
{
    QChar   label1 ('a');
    QChar   label2 ('b');
    QString output1("a");
    QString prefix1("x");
    QString final1 ("final1");
    QString empty  ("");

    State state1;
    QCOMPARE(state1.isFinal(), false);
    QCOMPARE(state1.transitions()->size() , 0);
    QCOMPARE(state1.finalStrings()->size(), 0);
    QCOMPARE(state1.next(label1) == NULL, true);

    state1.setFinal(true);
    QCOMPARE(state1.isFinal(), true);

    state1.updateFinalsWithPrefix(final1);
    QCOMPARE(state1.finalStrings()->size(), 1);
    QCOMPARE(state1.finalStrings()->at(0), final1);
    QCOMPARE(state1.key() > 0, true);

    state1.setOutput(label1, output1);
    QCOMPARE(state1.output(label1), empty);

    State state2;
    state2.setNext  (label1, &state1);
    state2.setOutput(label1, output1);
    QCOMPARE(state2.next(label1) == &state1, true);
    QCOMPARE(state2.next(label2) == NULL, true);
    QCOMPARE(state2.transitions()->size(), 1);
    QCOMPARE(state2.output(label1), output1);
    QCOMPARE(state2.output(label2), empty);

    state2.updateOutputsWithPrefix(prefix1);
    QCOMPARE(state2.output(label1), prefix1 + output1);
    QCOMPARE(state2.output(label2), empty);

    // FIXME: Assignment
}

void TestTransducer::simpleTransducer()
{
    QTemporaryFile transducer_source;
    transducer_source.open();
    transducer_source.write("lexeme\tlexeme\n");
    transducer_source.write("lexemes\tlexeme\n");
    transducer_source.close();

    TransducerManager t_manager1;
//    t_manager1.build(transducer_source.fileName(), 10);

//    QCOMPARE(t_manager1.transducer()->search("aaaaaa"       ).size(), 0);
//    QCOMPARE(t_manager1.transducer()->search("lexeme"       ).size(), 1);
//    QCOMPARE(t_manager1.transducer()->search("lexemes"      ).size(), 1);
//    QCOMPARE(t_manager1.transducer()->search("lex"          ).size(), 0);
//    QCOMPARE(t_manager1.transducer()->search("lexemezzz"    ).size(), 0);

//    t_manager1.save("./lexemes.qutd");

//    TransducerManager t_manager2;
//    t_manager2.load("./lexemes.qutd");

//    QCOMPARE(t_manager2.transducer()->search("aaaaaa"       ).size(), 0);
//    QCOMPARE(t_manager2.transducer()->search("lexeme"       ).size(), 1);
//    QCOMPARE(t_manager2.transducer()->search("lexemes"      ).size(), 1);
//    QCOMPARE(t_manager2.transducer()->search("lex"          ).size(), 0);
//    QCOMPARE(t_manager2.transducer()->search("lexemezzz"    ).size(), 0);
}

QTEST_MAIN(TestTransducer)
#include "test_transducer.moc"
