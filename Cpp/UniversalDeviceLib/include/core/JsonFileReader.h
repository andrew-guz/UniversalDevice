#ifndef _JSON_FILE_READER_H_
#define _JSON_FILE_READER_H_

#include <string_view>
#include <nlohmann/json.hpp>

class JsonFileReader final
{
public:
    static nlohmann::json ReadJson(std::string_view fullFileName);
};

#endif //_JSON_FILE_READER_H_
