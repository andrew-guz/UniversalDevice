#ifndef _URL_HELPER_H_
#define _URL_HELPER_H_

#include <string>
#include <regex>

#include "Uuid.h"
#include "Defines.h"

class UrlHelper final
{
public:
    static std::string Url(const std::string& url, const std::string& before, const std::string& after)
    {
        return std::regex_replace(url, std::regex(before), after);
    }
};

#endif //_URL_HELPER_H_
