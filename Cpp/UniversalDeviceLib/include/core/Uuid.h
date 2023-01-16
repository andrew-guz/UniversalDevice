#ifndef _UUID_H_
#define _UUID_H_

#include <uuid/uuid.h>
#include <string>

class Uuid final
{
public:
    Uuid();

    Uuid(const Uuid& other);

    Uuid(const std::string& str);

    ~Uuid() = default;

    Uuid& operator=(const Uuid& other);

    bool operator==(const Uuid& other) const;

    bool operator!=(const Uuid& other) const;

    bool operator<(const Uuid& other) const;

    std::string data() const;

private:
  uuid_t _uuid;
};

#endif //_UUID_H_
