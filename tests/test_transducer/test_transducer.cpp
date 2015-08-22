#include <QtTest/QtTest>
#include <qubiq/util/transducer_state_transition.h>
#include <qubiq/util/transducer_state.h>
#include <qubiq/util/transducer.h>
#include <qubiq/util/transducer_manager.h>

class TestTransducer: public QObject
{
    Q_OBJECT

private slots:
    void testStateTransition();
    void testTransducerStates();
    void testStateHashing();
    void simpleTransducer();
};

void TestTransducer::testStateTransition()
{
    QChar   label('a');
    QString empty("");

    Transition t1(label);

    QCOMPARE(t1.label(), label);
    QCOMPARE(t1.output(), empty);
    QCOMPARE(t1.next() == NULL, true);

    Transition t2(t1);
    QCOMPARE(t2.label(), label);
    QCOMPARE(t2.output(), empty);
    QCOMPARE(t2.next() == NULL, true);

    State s1, s2;

    t1.setNext(&s1);
    t2.setNext(&s2);
    QCOMPARE(t1.next() == &s1, true);
    QCOMPARE(t2.next() == &s2, true);

    Transition t3 = t1;
    QCOMPARE(t3.label(), label);
    QCOMPARE(t3.output(), empty);
    QCOMPARE(t3.next() == &s1, true);

    QString prefix("prefix");
    t3.prependOutput(prefix);
    QCOMPARE(t3.output(), prefix);
    t3.setNext(NULL);

    QCOMPARE(qHash(t1) != qHash(t2), true);
    QCOMPARE(qHash(t1) != qHash(t3), true);
    QCOMPARE(qHash(t2) != qHash(t3), true);
}

void TestTransducer::testTransducerStates()
{
    /* Topology of states being tested:
     *
     *  +-(l1,o1)---->fs1[f1,f2,f3]
     *  |
     * s1-(l2,o2)---->fs2[f4]
     *  |               |
     *  +-(l3,o3)-->s2  +-(l4,o4)-->fs3[f5]
     *               |
     *               +-(l5,o5)-->fs4[f4]
     *
     */
    QChar l1('a'), l2('b'), l3('c'), l4('d'), l5('e');

    QString o1("qw"), o2("er"), o3("ty"), o4("ui"), o5("op");
    QString f1("f1"), f2("F2"), f3("F3"), f4("F4"), f5("F5");

    QString prefix("x");
    QString empty ("");

    State s1, s2, fs1, fs2, fs3, fs4;

    // State right after default construction:
    QCOMPARE(s1.isFinal(), false);
    QCOMPARE(s1.transitions()->size() , 0);
    QCOMPARE(s1.finalStrings()->size(), 0);
    QCOMPARE(s1.next(l1) == NULL, true);

    fs1.setFinal(true);
    fs2.setFinal(true);
    fs3.setFinal(true);
    fs4.setFinal(true);
    QCOMPARE(fs1.isFinal(), true);
    QCOMPARE(fs2.isFinal(), true);
    QCOMPARE(fs3.isFinal(), true);
    QCOMPARE(fs4.isFinal(), true);

    fs1.addFinal(f1);
    fs1.addFinal(f2);
    fs1.addFinal(f3);
    QCOMPARE(fs1.finalStrings()->size(), 3);
    QCOMPARE(fs1.finalStrings()->at(0), f2);
    QCOMPARE(fs1.finalStrings()->at(1), f3);
    QCOMPARE(fs1.finalStrings()->at(2), f1);

    fs1.updateFinalsWithPrefix(prefix); // FIXME: Test on empty set
    QCOMPARE(fs1.finalStrings()->size(), 3);
    QCOMPARE(fs1.finalStrings()->at(0), prefix + f2);
    QCOMPARE(fs1.finalStrings()->at(1), prefix + f3);
    QCOMPARE(fs1.finalStrings()->at(2), prefix + f1);

    fs2.addFinal(f4);
    fs3.addFinal(f5);
    fs4.addFinal(f4);

    // Setting output on non-existent label does not create a transition:
    s1.setOutput(l1, o1);
    QCOMPARE(s1.output(l1), empty);
    QCOMPARE(s1.next(l1) == NULL, true);

    s1.setNext  (l1, &fs1);
    s1.setOutput(l1, o1);
    s1.setNext  (l2, &fs2);
    s1.setOutput(l2, o2);
    s1.setNext  (l3, &s2);
    s1.setOutput(l3, o3);

    QCOMPARE(s1.next(l1)  == &fs1, true);
    QCOMPARE(s1.next(l2)  == &fs2, true);
    QCOMPARE(s1.next(l3)  ==  &s2, true);
    QCOMPARE(s1.next('z') == NULL, true);
    QCOMPARE(s1.output(l1), o1);

    fs2.setNext  (l4, &fs3);
    fs2.setOutput(l4, o4);

    s2.setNext  (l5, &fs4);
    s2.setOutput(l5, o5);

    QCOMPARE( s1.transitions()->size(), 3);
    QCOMPARE( s2.transitions()->size(), 1);
    QCOMPARE(fs1.transitions()->size(), 0);
    QCOMPARE(fs2.transitions()->size(), 1);
    QCOMPARE(fs3.transitions()->size(), 0);
    QCOMPARE(fs4.transitions()->size(), 0);

    QCOMPARE( s1.key() !=  s2.key(), true);
    QCOMPARE(fs1.key() != fs2.key(), true);
    QCOMPARE(fs2.key() != fs3.key(), true);
    QCOMPARE(fs3.key() != fs4.key(), true);

    // Same final suffixes, but fs2 has transitions as well:
    QCOMPARE(fs2.key() != fs4.key(), true);

    //
    // Equivalent states by the manner they were built:
    //

    State s1_equiv;
    s1_equiv.setNext  (l3, &s2);
    s1_equiv.setOutput(l3, o3);
    s1_equiv.setNext  (l2, &fs2);
    s1_equiv.setOutput(l2, o2);
    s1_equiv.setNext  (l1, &fs1);
    s1_equiv.setOutput(l1, o1);
    QCOMPARE(s1.key(), s1_equiv.key());

    State fs1_equiv;
    fs1_equiv.setFinal(true);
    fs1_equiv.addFinal(f3);
    fs1_equiv.addFinal(f2);
    fs1_equiv.addFinal(f1);

    QCOMPARE(fs1_equiv.addFinal(f1), false);

    fs1_equiv.updateFinalsWithPrefix(prefix);
    QCOMPARE(fs1.key(), fs1_equiv.key());

    //
    // Equivalent states by copying / assignment
    //

    State s1_copy = s1;
    QCOMPARE(s1.key(), s1_copy.key());

    State s2_copy(s2);
    QCOMPARE(s2.key(), s2_copy.key());

    State fs1_copy = fs1;
    QCOMPARE(fs1.key(), fs1_copy.key());

    State fs2_copy(fs2);
    QCOMPARE(fs2.key(), fs2_copy.key());
}

void TestTransducer::testStateHashing()
{
    /* Topology of states being tested:
     *
     * s1-(l1,o1)->s2-(l2,"")->s3-(l3,"")->s4-(l4,"")->s5-(l5,"")->s6-(l6,"")->fs1[]
     *
     */
    QChar l1('l'), l2('e'), l3('x'), l4('e'), l5('m'), l6('e');

    State s1, s2, s3, s4, s5, s6, fs1;

    fs1.setFinal(true);
    s6.setNext(l6, &fs1);
    s5.setNext(l5, &s6);
    s4.setNext(l4, &s5);
    s3.setNext(l3, &s4);
    s2.setNext(l2, &s3);
    s1.setNext(l1, &s2);

    QCOMPARE(s2.key() != s4.key(), true);
    QCOMPARE(s2.key() != s6.key(), true);
    QCOMPARE(s4.key() != s6.key(), true);
    QCOMPARE(s6.key() != fs1.key(), true);
}

void TestTransducer::simpleTransducer()
{
    QTemporaryFile transducer_source;
    transducer_source.open();
    transducer_source.write("lexeme\tlexeme\n");
    transducer_source.write("lexemes\tlexeme\n");
    transducer_source.close();

    TransducerManager t_manager1;
    QCOMPARE(t_manager1.build(transducer_source.fileName(), 10), true);

    QCOMPARE(t_manager1.transducer()->search("aaaaaa"       ).size(), 0);
    QCOMPARE(t_manager1.transducer()->search("lexeme"       ).size(), 1);
    QCOMPARE(t_manager1.transducer()->search("lexemes"      ).size(), 1);
    QCOMPARE(t_manager1.transducer()->search("lex"          ).size(), 0);
    QCOMPARE(t_manager1.transducer()->search("lexemezzz"    ).size(), 0);

    QCOMPARE(t_manager1.save("./lexemes.qutd"), true);

    TransducerManager t_manager2;
    QCOMPARE(t_manager2.load("./lexemes.qutd"), true);

    QCOMPARE(t_manager2.transducer()->search("aaaaaa"       ).size(), 0);
    QCOMPARE(t_manager2.transducer()->search("lexeme"       ).size(), 1);
    QCOMPARE(t_manager2.transducer()->search("lexemes"      ).size(), 1);
    QCOMPARE(t_manager2.transducer()->search("lex"          ).size(), 0);
    QCOMPARE(t_manager2.transducer()->search("lexemezzz"    ).size(), 0);
}

QTEST_MAIN(TestTransducer)
#include "test_transducer.moc"
