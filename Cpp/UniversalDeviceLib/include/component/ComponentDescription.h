#ifndef _COMPONENT_DESCRIPTION_H_
#define _COMPONENT_DESCRIPTION_H_

#include <string>

#include "IJson.h"
#include "Constants.h"
#include "Uuid.h"

struct ComponentDescription : public IJson<ComponentDescription>
{
    std::string _type;
    Uuid        _id;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "type", _type },
            { "id", _id.data() }
        };
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _type = json.value("type", "");
        _id = Uuid(json.value("id", ""));
    }

    bool operator<(const ComponentDescription& other) const
    {
        return make_tuple(_id, _type) < std::make_tuple(other._id, other._type);
    }
};

#endif //_COMPONENT_DESCRIPTION_H_
