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
            { "timestamp", TimeHelper::TimeToInt(_timestamp) }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
    }
};

#endif //_CURRENT_TIME_H_
