

#include "RelaySimulator.hpp"

#include <ixwebsocket/IXWebSocketMessage.h>
#include <ixwebsocket/IXWebSocketMessageType.h>
#include <nlohmann/json_fwd.hpp>

#include "Enums.hpp"
#include "MessageHelper.hpp"
#include "PeriodSettings.hpp"
#include "RelayValue.hpp"
#include "Simulator.hpp"

RelaySimulator::RelaySimulator() :
    Simulator(DeviceType::Relay) {}

bool RelaySimulator::GetState() const { return state; }

int RelaySimulator::GetPeriod() const { return periodSettings._period; }

void RelaySimulator::SendState() {
    RelayValue stateValue;
    stateValue._state = GetState();
    auto stateMessage = MessageHelper::CreateDeviceMessage(GetType(), GetId(), Subject::RelayCurrentState, stateValue);
    SendMessage(stateMessage);
}

void RelaySimulator::OnMessage(const ix::WebSocketMessagePtr& message) {
    if (message->type == ix::WebSocketMessageType::Message) {
        try {
            auto json = nlohmann::json::parse(message->str);
            if (json.contains("period")) {
                periodSettings = json.get<PeriodSettings>();
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
