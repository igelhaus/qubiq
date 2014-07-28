#include <QtTest/QtTest>

#include <qubiq/lexeme.h>

class TestLexeme: public QObject
{
    Q_OBJECT

private slots:
    void createLexeme();
};

void TestLexeme::createLexeme()
{
    QString main_form("lexeme");
    Lexeme lexeme(main_form, false);

    QCOMPARE(lexeme.lexeme(), main_form);
    QCOMPARE(lexeme.isBoundary(), false);
    QCOMPARE(lexeme.forms()->length(), 0);
}

QTEST_MAIN(TestLexeme)
#include "test_lexeme.moc"
