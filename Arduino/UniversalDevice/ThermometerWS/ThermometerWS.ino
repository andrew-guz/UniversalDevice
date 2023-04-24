#include <Arduino.h>
#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "WebSocketHelper.h"
#include "MessageHelper.h"
#include "TemperatureHelper.h"
#ifdef USE_LED
#include <TM1637TinyDisplay.h>
#endif

SingleTemperatureSensor temperatureSensor(D6);
#ifdef USE_LED
TM1637TinyDisplay display(D4, D5);
#endif
unsigned long temperatureStartTime;
int measurementDelay = 5000;
#ifdef USE_LED
int ledBrightness = BRIGHT_7;
#else 
int ledBrightness = 0;
#endif

void ledSetBrightness(int value)
{
#ifdef USE_LED
    if (value != BRIGHT_0)
      display.setBrightness(value, true);
    else
      display.setBrightness(value, false);
#endif   
}

void ledShowString(const String& str)
{
#ifdef USE_LED
    display.showString(str.c_str());
#endif    
}

void ledShowTemperature(float temperature)
{
#ifdef USE_LED  
    display.showNumber(temperature, 1);
#endif
}

void sendTemperature(float temperature)
{
    auto message = CreateSimpleMessage("thermometer", UUID, "thermometer_current_value", "value", temperature);
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
          auto authMessage = CreateSimpleMessage("thermometer", UUID, "websocket_authorization", "authString", AUTHORIZATION_STR);
          webSocket.sendTXT(authMessage);
          auto settingsMessage = CreateSimpleMessage("thermometer", UUID, "websocket_get_settings");
          webSocket.sendTXT(settingsMessage);
          auto commandsMessage = CreateSimpleMessage("thermometer", UUID, "websocket_get_commands");
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
              measurementDelay = doc["period"].as<int>();
          else if (doc.containsKey("brightness"))
          {
              ledBrightness = doc["brightness"].as<int>();
              ledSetBrightness(ledBrightness);
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

    temperatureSensor.Setup();

    ledSetBrightness(ledBrightness);
    ledShowString("HELO");

    WebSocketHelper::Configure(WebSocketEvent);
}

void loop()
{
    //check the connection
    if (WiFi.status() != WL_CONNECTED)
    {
        ledShowString("CON-");
        bool connected = wifiHelper.WiFiConnect();
        if (!connected)
        {            
            ledShowString("EROR");
            delay(1000);
            return;
        }
        ledShowString("CONN");
        WebSocketHelper::Connect();
        temperatureStartTime = millis();
    }

    webSocket.loop();

    auto currentTime = millis();

    if (currentTime <= temperatureStartTime)
    {
        //case when millis goes over 0 - once in rough 50 days
        temperatureStartTime = millis();
        return;
    }

    //500 for measure time 
    if ((int)(currentTime - temperatureStartTime) >= measurementDelay - 500)
    {
        //last for 500 ms
        auto temperature = temperatureSensor.GetTemperature();
        ledShowTemperature(temperature);
        sendTemperature(temperature);
        temperatureStartTime = currentTime;
    }
}
