#ifndef _NIGHT_MODE_DEVICE_H_
#define _NIGHT_MODE_DEVICE_H_

#include "IDb.h"
#include "Logger.h"
#include "DbExtension.h"
#include "TimeHelper.h"
#include "ComponentDescription.h"
#include "NightModeTiming.h"

struct NightModeDevice final : public IDb<NightModeDevice>
{
    ComponentDescription    _description;
    NightModeTiming         _timing;

    virtual std::vector<std::string> ToDbStrings() const override
    {
        //id, type, start, end
        return {
            _description._id.data(),
            _description._type,
            std::to_string(TimeHelper::TimeToInt(_timing._start)),
            std::to_string(TimeHelper::TimeToInt(_timing._end))
        };
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override
    {
        if (dbStrings.size() != 4)
        {
            auto id = DbExtension::FindValueByName(dbStrings, "id");
            auto type = DbExtension::FindValueByName(dbStrings, "type");
            auto start = DbExtension::FindValueByName(dbStrings, "start");
            auto end = DbExtension::FindValueByName(dbStrings, "end");            
            if (id.size() &&
                type.size() &&
                start.size() &&
                end.size())
            {
                _description._id = id;
                _description._type = type;
                _timing._start = TimeHelper::TimeFromInt((int64_t)std::stoll(start));
                _timing._end = TimeHelper::TimeFromInt((int64_t)std::stoll(end));
            }
        }
        else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};

#endif //_NIGHT_MODE_DEVICE_H_
