#pragma once

#include <ixwebsocket/IXWebSocket.h>

#include "BaseParameters.hpp"
#include "Message.hpp"

class Simulator {
protected:
    Simulator(const std::string_view type);

public:
    virtual ~Simulator();

    std::string_view GetType() const;

    const Uuid& GetId() const;

    void AskForSettings();

    void AskForCommands();

    void SendMessage(const Message& message);

protected:
    virtual void OnMessage(const ix::WebSocketMessagePtr& msg) = 0;

private:
    const std::string_view _type;
    BaseParameters _parameters;
    ix::WebSocket _websocket;
};
