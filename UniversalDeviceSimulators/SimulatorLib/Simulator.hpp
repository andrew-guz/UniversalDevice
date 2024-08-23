#pragma once

#include <ixwebsocket/IXWebSocket.h>

#include "BaseParameters.hpp"
#include "Enums.hpp"
#include "Message.hpp"

class Simulator {
protected:
    Simulator(DeviceType type);

public:
    virtual ~Simulator();

    DeviceType GetType() const;

    const Uuid& GetId() const;

    void AskForSettings();

    void AskForCommands();

    void SendMessage(const Message& message);

protected:
    virtual void OnMessage(const ix::WebSocketMessagePtr& msg) = 0;

private:
    const DeviceType _type;
    BaseParameters _parameters;
    ix::WebSocket _websocket;
};
