#include "Simulator.hpp"

#include <sstream>

#include <ixwebsocket/IXSocketTLSOptions.h>

#include "Constants.hpp"
#include "Defines.hpp"
#include "Marshaling.hpp"
#include "MessageHelper.hpp"
#include "WebSocketAuthentication.hpp"

Simulator::Simulator(const DeviceType type) : _type(type) {
    _parameters = BaseParameters::ReadFromFile();

    std::stringstream urlStream;
    urlStream << "wss://localhost:" << _parameters._port << API_DEVICE_WEBSOCKETS;
    auto url = urlStream.str();

    _websocket.setUrl(url);
    // The way to ignore certificate
    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "NONE";
    _websocket.setTLSOptions(tlsOptions);
    _websocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& message) {
        if (message->type == ix::WebSocketMessageType::Open) {
            WebSocketAuthentication auth;
            auth._authString = "Basic ZGV2aWNlOmVBITdSPWgmVnU=";
            auto authMessage = MessageHelper::Create(_type, _parameters._id, Constants::SubjectWebSocketAuthorization, auth);
            _websocket.send(nlohmann::json(authMessage).dump());
            AskForSettings();
            AskForCommands();
        } else if (message->type == ix::WebSocketMessageType::Message)
            OnMessage(message);
    });
    _websocket.start();
}

Simulator::~Simulator() { _websocket.close(); }

DeviceType Simulator::GetType() const { return _type; }

const Uuid& Simulator::GetId() const { return _parameters._id; }

void Simulator::AskForSettings() {
    auto settingsMessage = MessageHelper::Create(_type, _parameters._id, Constants::SubjectWebSocketGetSettings, {});
    _websocket.send(nlohmann::json(settingsMessage).dump());
}

void Simulator::AskForCommands() {
    auto commandsMessage = MessageHelper::Create(_type, _parameters._id, Constants::SubjectWebSocketGetCommands, {});
    _websocket.send(nlohmann::json(commandsMessage).dump());
}

void Simulator::SendMessage(const Message& message) { _websocket.send(nlohmann::json(message).dump()); }

void Simulator::OnMessage(const ix::WebSocketMessagePtr& message) {}
