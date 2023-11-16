#include <Arduino.h>
#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "WebSocketHelper.h"
#include "MessageHelper.h"
#ifdef HAS_THERMOMETER
  #include "TemperatureHelper.h"
#endif
#ifdef HAS_LED
  #include <TM1637TinyDisplay.h>
#endif
#ifdef HAS_RELAY
  #include "RelayHelper.h"
#endif
#ifdef HAS_MOTION_RELAY
  #include "MotionHelper.h"
  #include "RelayHelper.h"
#endif

#ifdef HAS_THERMOMETER
  SingleTemperatureSensor temperatureSensor(THERMOMETER_PIN);
  unsigned long temperatureStartTime;
  int temperatureMeasurementDelay = 5000;
#endif 
#ifdef HAS_LED
  TM1637TinyDisplay ledDisplay(LED_CLK_PIN, LED_DIO_PIN);
  int ledBrightness = BRIGHT_7;
#endif
#ifdef HAS_RELAY
  RelayHelper relayHelper(RELAY_PIN);
  unsigned long relayStartTime;
  int relayCheckStateDelay = 5000;
  int relayStateFromCommand = 0;
  #ifdef RELAY_AS_THERMOSTAT
    SingleTemperatureSensor temperatureSensor(RELAY_THERMOSTAT_PIN);
  #endif
#endif
#ifdef HAS_MOTION_RELAY
  MotionHelper motionHelper(MOTION_RELAY_DETECTOR_PIN);
  RelayHelper relayHelper(MOTION_RELAY_RELAY_PIN);
  unsigned long relayStartTime;
  int relayCheckStateDelay = 5000;
  int motionActivityDelay = 60000;
  int relayStateFromCommand = 0;
  bool motionState = false;
  unsigned long motionTime;
#endif

#ifdef HAS_THERMOMETER
  void sendTemperature(float temperature)
  {
    auto message = CreateSimpleMessage("thermometer", UUID, "thermometer_current_value", "value", temperature);
    webSocket.sendTXT(message);
  }
#endif

#ifdef HAS_LED
  void ledSetBrightness(int value)
  {
    if (value != BRIGHT_0)
      ledDisplay.setBrightness(value, true);
    else
      ledDisplay.setBrightness(value, false);
  }

  void ledShowString(const String& str)
  {
    ledDisplay.showString(str.c_str());
  }

  void ledShowTemperature(float temperature)
  {
    ledDisplay.showNumber(temperature, 1);
  }
#endif

#ifdef HAS_RELAY
  void sendRelayState()
  {
    auto message = CreateSimpleMessage("relay", UUID, "relay_current_state", "state", relayHelper.State());
    webSocket.sendTXT(message);
  }
#endif

#ifdef HAS_MOTION_RELAY
  void sendMotionRelayState()
  {
    auto func = [](DynamicJsonDocument& doc)
    {        
      doc["data"]["motion"] = motionState;
      doc["data"]["state"] = relayHelper.State();        
    };
    auto message = CreateMessage("motion_relay", UUID, "motion_relay_current_state", func);
    webSocket.sendTXT(message);
  }

  void setRelayState()
  {
    if (relayStateFromCommand == 1 ||
        motionState == true)
      relayHelper.On();
    else
      relayHelper.Off();
  }
#endif

char websocketBuffer[256];
bool websocketConnected = false;

void WebSocketEvent(WStype_t type, uint8_t* payload, size_t length)
{
  switch(type)
  {
  case WStype_DISCONNECTED:
    websocketConnected = false;
    WebSocketHelper::Connect();
    break;
  case WStype_CONNECTED:
    {
      websocketConnected = true;
      String type;
      #ifdef HAS_THERMOMETER
        type = "thermometer";
      #endif
      #ifdef HAS_RELAY
        type = "relay";
      #endif
      #ifdef HAS_MOTION_RELAY
        type = "motion_relay";
      #endif
      auto authMessage = CreateSimpleMessage(type, UUID, "websocket_authorization", "authString", AUTHORIZATION_STR);
      webSocket.sendTXT(authMessage);
      auto settingsMessage = CreateSimpleMessage(type, UUID, "websocket_get_settings");
      webSocket.sendTXT(settingsMessage);
      auto commandsMessage = CreateSimpleMessage(type, UUID, "websocket_get_commands");
      webSocket.sendTXT(commandsMessage);
    }
    break;
  case WStype_TEXT:
    {
      memset(websocketBuffer, 1, 256);
      sprintf(websocketBuffer, "%s", payload);
      StaticJsonDocument<256> doc;
      deserializeJson(doc, websocketBuffer);
      #ifdef HAS_THERMOMETER
        if (doc.containsKey("period"))
          temperatureMeasurementDelay = doc["period"].as<int>();
        #ifdef HAS_LED
          if (doc.containsKey("brightness"))
          {
            ledBrightness = doc["brightness"].as<int>();
            ledSetBrightness(ledBrightness);
          }
        #endif
      #endif
      #ifdef HAS_RELAY
        if (doc.containsKey("period"))
          relayCheckStateDelay = doc["period"].as<int>();
        if (doc.containsKey("state"))
        {
          relayStateFromCommand = doc["state"].as<int>();
          relayStateFromCommand ? relayHelper.On() : relayHelper.Off();
          sendRelayState();
        }
      #endif
      #ifdef HAS_MOTION_RELAY
        if (doc.containsKey("period"))
          relayCheckStateDelay = doc["period"].as<int>();
        if (doc.containsKey("activityTime"))
          motionActivityDelay = doc["activityTime"].as<int>();
        if (doc.containsKey("state"))
        {
          relayStateFromCommand = doc["state"].as<int>();
          setRelayState();
        }
      #endif
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

  #ifdef HAS_THERMOMETER
    temperatureSensor.Setup();
  #endif

  #ifdef HAS_LED
    ledSetBrightness(ledBrightness);
    ledShowString("HELO");
  #endif

  #ifdef HAS_RELAY
    relayHelper.Off();
    #ifdef RELAY_AS_THERMOSTAT
      temperatureSensor.Setup();
    #endif
  #endif

  #ifdef HAS_MOTION_RELAY
    relayHelper.Off();
    //sleep for 1 minute to be sure that motion sensor initialized
    Serial.println("Initializing motion sensor...");
    delay(60000);
    Serial.println("Motion sensor initializes...");
  #endif

  WebSocketHelper::Configure(WebSocketEvent);
}

void loop()
{
  //check the connection
  if (WiFi.status() != WL_CONNECTED)
  {
    #ifdef HAS_LED
      ledShowString("CON-");
    #endif
    bool connected = wifiHelper.WiFiConnect();
    if (!connected)
    {
      #ifdef HAS_LED
        ledShowString("EROR");
      #endif
      #if defined HAS_RELAY && defined RELAY_AS_THERMOSTAT
        auto currentTemperature = temperatureSensor.GetTemperature();
        if (currentTemperature < RELAY_THERMOSTAT_VALUE - RELAY_THERMOSTAT_DELTA)
        {
          if (relayHelper.State() == 0)
            relayHelper.On();
        }
        if (currentTemperature > RELAY_THERMOSTAT_VALUE + RELAY_THERMOSTAT_DELTA)
        {
          if (relayHelper.State() == 1)
            relayHelper.Off();
        }
      #endif
      delay(1000);
      return;
    }
    #ifdef HAS_LED
      ledShowString("CONN");
    #endif
    WebSocketHelper::Connect();
    #ifdef HAS_THERMOMETER
      temperatureStartTime = millis();
    #endif
    #ifdef HAS_RELAY
      relayStartTime = millis();
    #endif
    #ifdef HAS_MOTION_RELAY
      motionTime = relayStartTime = millis();
    #endif
  }

  webSocket.loop();

  auto currentTime = millis();

  //case when millis goes over 0 - once in rough 50 days
  #ifdef HAS_THERMOMTER
    if (currentTime <= temperatureStartTime)
    {
      temperatureStartTime = millis();
      return;
    }
  #endif

  #ifdef HAS_RELAY
    if (currentTime <= relayStartTime)
    {
        relayStartTime = millis();
        return;
    }
  #endif

  #ifdef HAS_MOTION_RELAY
    if (currentTime <= relayStartTime)
    {      
      relayStartTime = millis();
      return;
    }
  #endif

  #ifdef HAS_THERMOMETER
  //500 for measure time 
  if ((int)(currentTime - temperatureStartTime) >= temperatureMeasurementDelay - 500)
  {
    //last for 500 ms
    auto temperature = temperatureSensor.GetTemperature();
    #ifdef HAS_LED
      ledShowTemperature(temperature);
    #endif
    sendTemperature(temperature);
    temperatureStartTime = currentTime;
  }
  #endif

  #ifdef HAS_RELAY
    if (currentTime - relayStartTime >= relayCheckStateDelay - 530)
    {
      //last for 500 ms
      sendRelayState();
      relayStartTime = currentTime;
    }
    #ifdef RELAY_AS_THERMOSTAT
    if (websocketConnected == false)
    {
      auto currentTemperature = temperatureSensor.GetTemperature();
      if (currentTemperature < RELAY_THERMOSTAT_VALUE - RELAY_THERMOSTAT_DELTA)
      {
        if (relayHelper.State() == 0)
        {
          relayHelper.On();
          sendRelayState();
        }
      }
      if (currentTemperature > RELAY_THERMOSTAT_VALUE + RELAY_THERMOSTAT_DELTA)
      {
        if (relayHelper.State() == 1)
        {
          relayHelper.Off();
          sendRelayState();
        }
      }
    }
    #endif
  #endif

  #ifdef HAS_MOTION_RELAY
    if (currentTime - relayStartTime >= relayCheckStateDelay)
    {
      sendMotionRelayState();
      relayStartTime = currentTime;
    }

    if (motionHelper.IsMotion())
    {
      if (motionState == false)
      {
        motionState = true;
        setRelayState();
        sendMotionRelayState();                    
      }
      motionTime = currentTime;
    }
    else
    {
      if (motionState &&
          currentTime - motionTime >= motionActivityDelay)
      {
        motionState = false;
        setRelayState();
        sendMotionRelayState();
      }
    }    
  #endif
}
