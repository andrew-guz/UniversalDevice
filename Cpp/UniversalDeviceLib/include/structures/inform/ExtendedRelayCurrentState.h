#ifndef _EXTENDED_RELAY_CURRENT_STATE_H_
#define _EXTENDED_RELAY_CURRENT_STATE_H_

#include <chrono>

#include "DbExtension.h"
#include "IDb.h"
#include "Logger.h"
#include "RelayCurrentState.h"
#include "TimeHelper.h"

struct ExtendedRelayCurrentState final : public RelayCurrentState, public IJson<ExtendedRelayCurrentState>, public IDb<ExtendedRelayCurrentState> {
    std::chrono::system_clock::time_point _timestamp;

    virtual nlohmann::json ToJson() const override {
        auto relayCurrentState = RelayCurrentState::ToJson();
        relayCurrentState += {"timestamp", TimeHelper::TimeToInt(_timestamp)};
        return relayCurrentState;
    }

    virtual void FromJson(const nlohmann::json& json) override {
        RelayCurrentState::FromJson(json);
        _timestamp = TimeHelper::TimeFromInt(json.value("timestamp", (int64_t)0));
    }

    virtual std::vector<std::string> ToDbStrings() const override {
        LOG_ERROR << "This object should not be written to Database." << std::endl;
        return {};
    }

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) override {
        if (dbStrings.size() % 2 == 0) {
            auto timestamp = DbExtension::FindValueByName(dbStrings, "timestamp");
            auto state = DbExtension::FindValueByName(dbStrings, "state");
            if (timestamp.size() && state.size()) {
                _timestamp = TimeHelper::TimeFromInt((int64_t)std::stoll(timestamp));
                _state = atoi(state.c_str());
            }
        } else
            LOG_ERROR << "Invalid db strings." << std::endl;
    }
};

#endif //_EXTENDED_RELAY_CURRENT_STATE_H_
