#include "Simulator.hpp"

#include <nlohmann/json.hpp>

#include "BaseParameters.hpp"
#include "Constants.h"
#include "Defines.h"
#include "MessageHelper.h"
#include "WebSocketAuthentication.h"

Simulator::Simulator(const std::string_view type) : _type(std::move(type)) {
    auto parameters = BaseParameters::ReadFromFile();
    _id = parameters._id;

    _websocket.setUrl(API_DEVICE_WEBSOCKETS);
    _websocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& message) {
        if (message->type == ix::WebSocketMessageType::Open) {
            WebSocketAuthentication auth;
            auth._authString = "Basic ZGV2aWNlOmVBITdSPWgmVnU=";
            auto authMessage = MessageHelper::Create(_type, _id, Constants::SubjectWebSocketAuthorization, auth);
            _websocket.send(authMessage.ToJson().dump());
        } else if (message->type == ix::WebSocketMessageType::Message)
            OnMessage(message);
    });
    _websocket.start();
}

Simulator::~Simulator() { _websocket.close(); }

std::string_view Simulator::GetType() const { return _type; }

const Uuid& Simulator::GetId() const { return _id; }

void Simulator::AskForSettings() {
    auto settingsMessage = MessageHelper::Create(_type, _id, Constants::SubjectWebSocketGetSettings);
    _websocket.send(settingsMessage.ToJson().dump());
}

void Simulator::AskForCommands() {
    auto commandsMessage = MessageHelper::Create(_type, _id, Constants::SubjectWebSocketGetCommands);
    _websocket.send(commandsMessage.ToJson().dump());
}

void Simulator::SendMessage(const Message& message) { _websocket.send(message.ToJson().dump()); }

void Simulator::OnMessage(const ix::WebSocketMessagePtr& message) {}
