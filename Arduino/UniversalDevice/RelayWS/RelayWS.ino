#include <Arduino.h>
#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "WebSocketHelper.h"
#include "MessageHelper.h"
#include "RelayHelper.h"

RelayHelper relayHelper(26);
unsigned long informStartTime;
int informDelay = 5000;
int stateFromCommand = 0;

void sendState()
{
    auto message = CreateSimpleMessage("relay", UUID, "relay_current_state", "state", relayHelper.State());
    webSocket.sendTXT(message);
}

char websocketBuffer[256];

void WebSocketEvent(WStype_t type, uint8_t* payload, size_t length)
{
  switch(type)
  {
  case WStype_DISCONNECTED:
      WebSocketHelper::Connect();
      break;
  case WStype_CONNECTED:
      {
          auto authMessage = CreateSimpleMessage("relay", UUID, "websocket_authorization", "authString", AUTHORIZATION_STR);
          webSocket.sendTXT(authMessage);
          auto settingsMessage = CreateSimpleMessage("relay", UUID, "websocket_get_settings");
          webSocket.sendTXT(settingsMessage);
          auto commandsMessage = CreateSimpleMessage("relay", UUID, "websocket_get_commands");
          webSocket.sendTXT(commandsMessage);
      }
      break;
  case WStype_TEXT:
      {
          memset(websocketBuffer, 1, 256);
          sprintf(websocketBuffer, "%s", payload);
          StaticJsonDocument<256> doc;
          deserializeJson(doc, websocketBuffer);
          if (doc.containsKey("period"))
              informDelay = doc["period"].as<int>();
          else if (doc.containsKey("state"))
          {
              stateFromCommand = doc["state"].as<int>();
              stateFromCommand ? relayHelper.On() : relayHelper.Off();
              sendState();
          }
        }
      break;
  case WStype_BIN:
      break;
  }
}

void setup()
{
    CheckDefines();

    Serial.begin(115200);

    relayHelper.Off();

    WebSocketHelper::Configure(WebSocketEvent);
}

void loop()
{
    //check the connection
    if (WiFi.status() != WL_CONNECTED)
    {
        bool connected = wifiHelper.WiFiConnect();
        if (!connected)
        {
            delay(1000);
            return;
        }
        WebSocketHelper::Connect();
        informStartTime = millis();
    }

    webSocket.loop();

    auto currentTime = millis();

    if (currentTime <= informStartTime)
    {
        //case when millis goes over 0 - once in rough 50 days
        informStartTime = millis();
        return;
    }

    if (currentTime - informStartTime >= informDelay - 530)
    {
        //last for 500 ms
        sendState();
        informStartTime = currentTime;
    }
}