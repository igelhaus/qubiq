#include <qubiq/util/lexeme.h>

Lexeme::Lexeme(const QString &name)
{
    _initialize(name, false);
}

//! Copy constructor. Constructs a Lexeme object from the other Lexeme object.
Lexeme::Lexeme(const Lexeme &other)
{
    _assign(other);
}

Lexeme::Lexeme(const QString &lexeme, bool is_boundary)
{
    _initialize(lexeme, is_boundary);
}

Lexeme::~Lexeme()
{
    _destroy();
}

bool Lexeme::addForm(const QString &form, int offset, bool overwrite /* = false */)
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

/**
 * \brief Assignment operator.
 * \param[in] other Another lexeme sequence to assign to the current object.
 * \returns Reference to the original object after assignment.
 */
Lexeme &Lexeme::operator =(const Lexeme &other)
{
    if (this != &other) {
        _destroy();
        _assign(other);
    }
    return *this;
}

//! \internal Initializes class members.
void Lexeme::_initialize(const QString &name, bool is_boundary)
{
    _lexeme      = name;
    _is_boundary = is_boundary;
    _forms       = new QVector<QString>();
    _offsets     = new QVector<int>();
    _idx_offsets = new QHash<int, int>();
}

//! \internal Assigns \c other members to \c this members.
void Lexeme::_assign(const Lexeme &other)
{
    _lexeme      = other._lexeme;
    _is_boundary = other._is_boundary;
    _forms       = new QVector<QString>(*(other._forms));
    _offsets     = new QVector<int>(*(other._offsets));
    _idx_offsets = new QHash<int, int>(*(other._idx_offsets));
}

//! \internal Frees memory occupied by class members.
void Lexeme::_destroy()
{
    delete _forms;
    delete _offsets;
    delete _idx_offsets;
}
