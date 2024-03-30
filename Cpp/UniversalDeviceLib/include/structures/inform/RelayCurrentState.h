#ifndef _RELAY_CURRENT_STATE_H_
#define _RELAY_CURRENT_STATE_H_

#include <limits>

#include "IJson.h"

struct RelayCurrentState : public IJson<RelayCurrentState> {
    int _state = std::numeric_limits<float>::min();

    virtual nlohmann::json ToJson() const override { return {{"state", _state}}; }

    virtual void FromJson(const nlohmann::json& json) override { _state = json.value("state", std::numeric_limits<float>::min()); }
};

#endif //_RELAY_CURRENT_STATE_H_
