#include <qubiq/lexeme.h>

Lexeme::Lexeme(const QString &lexeme, bool is_boundary)
{
    _lexeme      = lexeme;
    _is_boundary = is_boundary;
    _forms       = new QVector<QString>();
    _offsets     = new QVector<ulong>();
}

Lexeme::~Lexeme()
{
    delete _forms;
    delete _offsets;
}

void Lexeme::addForm(const QString &form, ulong offset)
{
    _forms->append(form);
    _offsets->append(offset);
}
