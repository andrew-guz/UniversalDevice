#ifndef _JSON_FILE_READER_H_
#define _JSON_FILE_READER_H_

#include <string>
#include <nlohmann/json.hpp>

class JsonFileReader final
{
public:
    static nlohmann::json ReadJson(const std::string& fullFileName);
};

#endif //_JSON_FILE_READER_H_
