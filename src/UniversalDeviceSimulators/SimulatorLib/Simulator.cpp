#include "Simulator.hpp"

#include <fmt/format.h>
#include <ixwebsocket/IXSocketTLSOptions.h>

#include "AccountManager.hpp"
#include "AccountManagerInitializer.hpp"
#include "Defines.hpp"
#include "Marshaling.hpp"
#include "MessageHelper.hpp"
#include "PathHelper.hpp"
#include "WebSocketAuthentication.hpp"

Simulator::Simulator(const DeviceType type) :
    _type(type) {
    _parameters = BaseParameters::ReadFromFile();

    AccountManager::Instance()->Init(std::make_shared<AccountManagerInitializer>(PathHelper::FullFilePath("authentication.json")));

    const auto url = fmt::format("wss://localhost:{}{}", _parameters._port, API_DEVICE_WEBSOCKETS);

    _websocket.setUrl(url);
    // The way to ignore certificate
    ix::SocketTLSOptions tlsOptions;
    tlsOptions.caFile = "NONE";
    _websocket.setTLSOptions(tlsOptions);
    _websocket.setOnMessageCallback([&](const ix::WebSocketMessagePtr& message) {
        if (message->type == ix::WebSocketMessageType::Open) {
            WebSocketAuthentication auth;
            auth._authString = "Basic ZGV2aWNlOmVBITdSPWgmVnU=";
            auto authMessage = MessageHelper::Create(_type, _parameters._id, Subject::WebSocketAuthorization, auth);
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
    auto settingsMessage = MessageHelper::Create(_type, _parameters._id, Subject::WebSocketGetSettings, {});
    _websocket.send(nlohmann::json(settingsMessage).dump());
}

void Simulator::AskForCommands() {
    auto commandsMessage = MessageHelper::Create(_type, _parameters._id, Subject::WebSocketGetCommands, {});
    _websocket.send(nlohmann::json(commandsMessage).dump());
}

void Simulator::SendMessage(const Message& message) { _websocket.send(nlohmann::json(message).dump()); }

void Simulator::OnMessage(const ix::WebSocketMessagePtr& message) {}
