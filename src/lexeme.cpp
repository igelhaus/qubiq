#include <qubiq/lexeme.h>

Lexeme::Lexeme(const QString &lexeme, bool is_boundary)
{
    _lexeme      = lexeme;
    _is_boundary = is_boundary;
    _forms       = new QVector<QString>();
    _offsets     = new QVector<ulong>();
    _idx_offsets = new QHash<ulong, int>;
}

Lexeme::~Lexeme()
{
    delete _forms;
    delete _offsets;
    delete _idx_offsets;
}

bool Lexeme::addForm(const QString &form, ulong offset, bool overwrite /* = false */)
{
    if (_idx_offsets->contains(offset)) {
        if (!overwrite)
            return false;
        _forms->replace(_idx_offsets->value(offset), form);
    } else {
        _forms->append(form);
        _offsets->append(offset);
        _idx_offsets->insert(offset, _forms->length() - 1);
    }
    return true;
}
