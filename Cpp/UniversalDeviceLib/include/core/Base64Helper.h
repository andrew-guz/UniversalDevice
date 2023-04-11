#ifndef _BASE64_HELPER_H_
#define _BASE64_HELPER_H_

#include <string>


class Base64Helper final
{
public:
    static std::string ToBase64(const std::string& str);

    static std::string FromBase64(const std::string& str);
};

#endif //_BASE64_HELPER_H_
