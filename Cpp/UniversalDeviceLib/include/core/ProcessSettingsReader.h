#ifndef _PROCESS_SETTINGS_READER_H_
#define _PROCESS_SETTINGS_READER_H_

#include <nlohmann/json.hpp>

class ProcessSettingsReader final
{
public:
    static nlohmann::json ReadProcessSettings();
};

#endif //_PROCESS_SETTINGS_READER_H_
