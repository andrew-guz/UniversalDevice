#include <Arduino.h>
#include <tuple>

#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "WebSocketHelper.h"
#include "MessageHelper.h"
#include "MotionHelper.h"
#include "RelayHelper.h"

//d1 5
//d2 4
MotionHelper motionHelper(5);
RelayHelper relayHelper(4);
unsigned long informStartTime;
int informDelay = 5000;
int activityDelay = 60000;
int stateFromCommand = 0;
bool motion = false;
unsigned long motionTime;

void sendState()
{
    auto func = [](DynamicJsonDocument& doc)
    {        
        doc["data"]["motion"] = motion;
        doc["data"]["state"] = relayHelper.State();        
    };
    auto message = CreateMessage("motion_relay", UUID, "motion_relay_current_state", func);
    webSocket.sendTXT(message);
}

int setRelayState()
{
    if (stateFromCommand == 1 ||
        motion == true)
    {
        relayHelper.On();
        return HIGH;
    }
    else
    {
        relayHelper.Off();
        return LOW;
    }
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
          auto authMessage = CreateSimpleMessage("motion_relay", UUID, "websocket_authorization", "authString", AUTHORIZATION_STR);
          webSocket.sendTXT(authMessage);
          auto settingsMessage = CreateSimpleMessage("motion_relay", UUID, "websocket_get_settings");
          webSocket.sendTXT(settingsMessage);
          auto commandsMessage = CreateSimpleMessage("motion_relay", UUID, "websocket_get_commands");
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
          if (doc.containsKey("activityTime"))
              activityDelay = doc["activityTime"].as<int>();
          if (doc.containsKey("state"))
          {
              stateFromCommand = doc["state"].as<int>();
              setRelayState();
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

    //sleep for 1 minute to be sure that motion sensor initialized
    delay(60000);

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

    if (currentTime - informStartTime >= informDelay)
    {
        sendState();
        informStartTime = currentTime;
    }

    if (motionHelper.IsMotion())
    {
        if (motion == false)
        {
            motion = true;
            setRelayState();
            sendState();            
        }
        motionTime = currentTime;
    }
    else
    {
        if (motion)
        {
            if (currentTime - motionTime >= activityDelay)
            {
                motion = false;
                setRelayState();
                sendState();
            }
        }   
    }    
}
