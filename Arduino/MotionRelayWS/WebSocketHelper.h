#ifndef _WEBSOCKET_HELPER_H_
#define _WEBSOCKET_HELPER_H_

#include <WebSocketsClient.h>

#include "Defines.h"

WebSocketsClient webSocket;

class WebSocketHelper
{
public:
  static void Configure(WebSocketsClient::WebSocketClientEvent websocketEventFunction)
  {
    webSocket.disableHeartbeat();
    webSocket.setReconnectInterval(5000);
    webSocket.onEvent(websocketEventFunction);
  }

  static void Connect()
  {
    webSocket.beginSSL(API_IP, API_PORT, API_WS);
  }
};

#endif //_WEBSOCKET_HELPER_H_
