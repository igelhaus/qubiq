#include <QtTest/QtTest>

#include <qubiq/util/lexeme.h>

class TestLexeme: public QObject
{
    Q_OBJECT

private slots:
    void createLexeme();
    void createBoundary();
    void fillWithForms();
};

void TestLexeme::createLexeme()
{
    QString main_form("lexeme");
    Lexeme lexeme(main_form, false);

    QCOMPARE(lexeme.lexeme(), main_form);
    QCOMPARE(lexeme.isBoundary(), false);
    QCOMPARE(lexeme.isVirtual(), true);
    QCOMPARE(lexeme.forms()->length(), 0);
    QCOMPARE(lexeme.offsets()->length(), 0);
}

void TestLexeme::createBoundary()
{
    QString main_form(".");
    Lexeme lexeme(main_form, true);

    QCOMPARE(lexeme.lexeme(), main_form);
    QCOMPARE(lexeme.isBoundary(), true);
    QCOMPARE(lexeme.isVirtual(), true);
    QCOMPARE(lexeme.forms()->length(), 0);
    QCOMPARE(lexeme.offsets()->length(), 0);
}

void TestLexeme::fillWithForms()
{
    QString main_form("write");
    Lexeme lexeme(main_form, false);

    QString
        form1("write"),
        form2("written"),
        form3("writes"),
        form4("writing"),
        form5("wrote")
    ;

    lexeme.addForm(form1,  5);
    lexeme.addForm(form2, 42);
    lexeme.addForm(form3, 60);
    QCOMPARE(lexeme.forms()->length(), 3);
    QCOMPARE(lexeme.offsets()->length(), 3);
    QCOMPARE(lexeme.forms()->at(1), form2);
    QCOMPARE(lexeme.offsets()->at(1), 42);

    lexeme.addForm(form4, 42);
    QCOMPARE(lexeme.forms()->length(), 3);
    QCOMPARE(lexeme.offsets()->length(), 3);
    QCOMPARE(lexeme.forms()->at(1), form2);
    QCOMPARE(lexeme.offsets()->at(1), 42);

    lexeme.addForm(form5, 42, true);
    QCOMPARE(lexeme.forms()->length(), 3);
    QCOMPARE(lexeme.offsets()->length(), 3);
    QCOMPARE(lexeme.forms()->at(1), form5);
    QCOMPARE(lexeme.offsets()->at(1), 42);
}

QTEST_MAIN(TestLexeme)
#include "test_lexeme.moc"
