#ifndef _I_JSON_H_
#define _I_JSON_H_

#include <nlohmann/json.hpp>

template<typename T>
class IJson {
public:
    IJson() = default;

    virtual ~IJson() = default;

    virtual nlohmann::json ToJson() const = 0;

    virtual void FromJson(const nlohmann::json& json) = 0;
};

#endif //_I_JSON_H_
