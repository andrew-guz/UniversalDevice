#ifndef _UUID_H_
#define _UUID_H_

#include <uuid/uuid.h>
#include <string>
#include <string_view>

class Uuid final
{
public:
    Uuid();

    Uuid(const Uuid& other);

    Uuid(std::string_view str);

    ~Uuid() = default;

    Uuid& operator=(const Uuid& other);

    Uuid& operator=(std::string_view other);

    bool operator==(const Uuid& other) const;

    bool operator!=(const Uuid& other) const;

    bool operator<(const Uuid& other) const;

    std::string data() const;

    bool isEmpty() const;

    static Uuid Empty();

private:
  uuid_t _uuid;
};

#endif //_UUID_H_
