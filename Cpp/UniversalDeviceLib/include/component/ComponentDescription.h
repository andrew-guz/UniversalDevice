#ifndef _COMPONENT_DESCRIPTION_H_
#define _COMPONENT_DESCRIPTION_H_

#include <string>

#include "Constants.h"
#include "IJson.h"
#include "Uuid.h"

struct ComponentDescription : public IJson<ComponentDescription> {
    std::string _type;
    Uuid _id;

    virtual nlohmann::json ToJson() const override { return {{"type", _type}, {"id", _id.data()}}; }

    virtual void FromJson(const nlohmann::json& json) override {
        _type = json.value("type", "");
        _id = Uuid(json.value("id", ""));
    }
};

#endif //_COMPONENT_DESCRIPTION_H_
