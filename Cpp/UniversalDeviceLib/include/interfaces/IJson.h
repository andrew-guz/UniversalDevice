#ifndef _I_JSON_H_
#define _I_JSON_H_

#include <nlohmann/json.hpp>
#include <vector>

template<typename T>
class IJson
{
public:
    IJson() = default;

    virtual ~IJson() = default;

    virtual nlohmann::json ToJson() const = 0;

    virtual void FromJson(const nlohmann::json& json) = 0;

    static T CreateFromJson(const nlohmann::json& json)
    {
        T t;
        t.FromJson(json);
        return t;
    }

    static std::vector<T> CreateVectorFromJson(const nlohmann::json& json)
    {
        std::vector<T> result;
        if (json.is_array())
            for (auto& j : json)
                result.push_back(CreateFromJson(j));
        return result;
    }
};

#endif //_I_JSON_H_
