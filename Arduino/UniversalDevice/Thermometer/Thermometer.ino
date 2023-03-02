#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"
#include "TemperatureHelper.h"
#ifdef USE_LED
#include <TM1637TinyDisplay.h>
#endif

SingleTemperatureSensor temperatureSensor(D6);
#ifdef USE_LED
TM1637TinyDisplay display(D4, D5);
#endif
unsigned long settingsCommandStartTime;
unsigned long temperatureStartTime;
int measurementDelay = 5000;
#ifdef USE_LED
int ledBrightness = BRIGHT_7;
#else 
int ledBrightness = 0;
#endif

//20ms
int getDelayFromSettings()
{
    String url = String(API_SETTINGS) + String("/") + String(UUID);
    auto replyString = wifiHelper.GetRequest(url);
    DynamicJsonDocument doc(128);
    auto error = deserializeJson(doc, replyString);
    if (!error &&
        doc.containsKey("period"))
        return doc["period"].as<int>();
    return measurementDelay; //return what I remember
}

//20ms
int getBrightnessFromCommands()
{
    String url = String(API_COMMANDS) + String("/") + String(UUID);
    auto replyString = wifiHelper.GetRequest(url);
    DynamicJsonDocument doc(128);
    auto error = deserializeJson(doc, replyString);
    if (!error &&
        doc.containsKey("brightness"))
        ledBrightness = doc["brightness"].as<int>();
    return ledBrightness;
}

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

//30 ms
void sendTemperature(float temperature)
{
    auto message = CreateSimpleMessage("thermometer", UUID, "thermometer_current_value", "value", temperature);
    wifiHelper.PostRequestNoData(API_INFORM, message);
}

void setup()
{
    CheckDefines();

    Serial.begin(115200);

    temperatureSensor.Setup();

    ledSetBrightness(ledBrightness);
    ledShowString("HELO");
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
        settingsCommandStartTime = temperatureStartTime = millis();
    }

    delay(100);

    auto currentTime = millis();

    if (currentTime <= settingsCommandStartTime ||
        currentTime <= temperatureStartTime)
    {
        //case when millis goes over 0 - once in rough 50 days
        settingsCommandStartTime = temperatureStartTime = millis();
        return;
    }

    if (currentTime - settingsCommandStartTime >= 500)
    {
        measurementDelay = getDelayFromSettings();
        auto brightnessFromCommand = getBrightnessFromCommands();
        ledSetBrightness(brightnessFromCommand);
        settingsCommandStartTime = currentTime;
    }

    //500 for measure time and 30 to send - so -530 ms
    if ((int)(currentTime - temperatureStartTime) >= measurementDelay - 530)
    {
        //last for 500 ms
        auto temperature = temperatureSensor.GetTemperature();
        ledShowTemperature(temperature);
        sendTemperature(temperature);
        temperatureStartTime = currentTime;
    }
}
