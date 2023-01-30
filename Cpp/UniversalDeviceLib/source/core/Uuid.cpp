#include "Uuid.h"

#include <cstring>

Uuid::Uuid()
{
    uuid_generate(_uuid);
}

Uuid::Uuid(const Uuid& other)
{
    uuid_copy(_uuid, other._uuid);
}

Uuid::Uuid(const std::string& str)
{
    uuid_parse(str.data(), _uuid);
}

Uuid& Uuid::operator=(const Uuid& other)
{
    uuid_copy(_uuid, other._uuid);
    return *this;
}

bool Uuid::operator==(const Uuid& other) const
{
    return uuid_compare(_uuid, other._uuid) == 0;
}

bool Uuid::operator!=(const Uuid& other) const
{
    return uuid_compare(_uuid, other._uuid) != 0;
}

bool Uuid::operator<(const Uuid& other) const
{
    return uuid_compare(_uuid, other._uuid) < 0;
}

std::string Uuid::data() const
{
    char buffer[37];
    std::memset(buffer, 0, 37);
    uuid_unparse_lower(_uuid, buffer);
    return std::string(buffer);
}

bool Uuid::isEmpty() const
{
    return uuid_is_null(_uuid);
}

Uuid Uuid::Empty()
{
    Uuid result;
    uuid_clear(result._uuid);
    return result;
}
