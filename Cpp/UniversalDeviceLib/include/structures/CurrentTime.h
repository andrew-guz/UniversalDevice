#ifndef _CURRENT_TIME_H_
#define _CURRENT_TIME_H_

#include "IJson.h"
#include "TimeHelper.h"

struct CurrentTime final : public IJson<CurrentTime>
{
    std::chrono::system_clock::time_point _timestamp;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "timestamp", TimeHelper::TimeToString(_timestamp) }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _timestamp = TimeHelper::TimeFromString(json.value("timestamp", ""));
    }
};

#endif //_CURRENT_TIME_H_
