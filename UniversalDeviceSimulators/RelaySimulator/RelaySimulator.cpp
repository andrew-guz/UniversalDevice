

#include "RelaySimulator.hpp"

#include <nlohmann/json.hpp>

#include "JsonExtension.h"
#include "MessageHelper.h"
#include "RelayCurrentState.h"

RelaySimulator::RelaySimulator() : Simulator("relay") {}

bool RelaySimulator::GetState() const { return state; }

int RelaySimulator::GetPeriod() const { return periodSettings._period; }

void RelaySimulator::SendState() {
    RelayCurrentState stateValue;
    stateValue._state = GetState();
    auto stateMessage = MessageHelper::Create(GetType(), GetId(), Constants::SubjectRelayCurrentState, stateValue);
    SendMessage(stateMessage);
}

void RelaySimulator::OnMessage(const ix::WebSocketMessagePtr& message) {
    if (message->type == ix::WebSocketMessageType::Message) {
        try {
            auto json = nlohmann::json::parse(message->str);
            if (json.contains("period")) {
                periodSettings = JsonExtension::CreateFromJson<PeriodSettings>(json);
                SendState();
            }
            if (json.contains("state")) {
                state = json["state"].get<int>();
                SendState();
            }
        } catch (...) {
        }
    }
}
