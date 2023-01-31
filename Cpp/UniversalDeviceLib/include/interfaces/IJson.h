#ifndef _I_JSON_H_
#define _I_JSON_H_

#include <nlohmann/json.hpp>
#include <vector>

#include "Logger.h"

template<typename T>
class IJson
{
public:
    IJson() = default;

    virtual ~IJson() = default;

    virtual nlohmann::json ToJson() const = 0;

    virtual void FromJson(const nlohmann::json& json) = 0;
};

class JsonExtension final
{
public:
    JsonExtension() = delete;

    ~JsonExtension() = default;

    template<typename T>
    static T CreateFromJson(const nlohmann::json& json)
    {
        T t;
        try
        {
            t.FromJson(json);
        }
        catch(...)
        {
            LOG_ERROR << "Failed to create from JSON " << json.dump() << "." << std::endl;
        }
        return t;
    }

    template<typename T>
    static std::vector<T> CreateVectorFromJson(const nlohmann::json& json)
    {
        std::vector<T> result;
        if (json.is_array())
        {
            try
            {
                for (auto& j : json)
                    result.push_back(CreateFromJson<T>(j));
            }
            catch(...)
            {
                LOG_ERROR << "Failed to create from JSON " << json.dump() << "." << std::endl;
            }
        }
        return result;
    }
};

#endif //_I_JSON_H_
