#ifndef _I_SERIALIZER_H_
#define _I_SERIALIZER_H_

#include <memory>
#include <nlohmann/json.hpp>

template<typename T>
class ISerializer
{
public:
    ISerializer() = default;

    virtual ~ISerializer() = default;

    virtual nlohmann::json ToJson(const T& t) const = 0;

    virtual nlohmann::json ToJson(const std::shared_ptr<T>& t) const = 0;

    virtual T ToObject(const nlohmann::json& j) const = 0;  

    virtual std::shared_ptr<T> ToSharedObject(const nlohmann::json& j) const = 0;
};

#endif //_I_SERIALIZER_H_
