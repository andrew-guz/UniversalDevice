#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"
#include "TemperatureHelper.h"
#include <TM1637TinyDisplay.h>

SingleTemperatureSensor temperatureSensor(D6);
TM1637TinyDisplay display(D4, D5);
unsigned long settingsStartTime;
unsigned long temperatureStartTime;
int measurementDelay = 5000;

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

void showTemperature(float temperature)
{
    display.showNumber(temperature, 1);
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

    display.setBrightness(BRIGHT_7);
    display.showString("HELO");
}

void loop()
{
    //check the connection
    if (WiFi.status() != WL_CONNECTED)
    {
        display.showString("CON-");
        bool connected = wifiHelper.WiFiConnect();
        if (!connected)
        {            
            display.showString("EROR");
            delay(1000);
            return;
        }
        display.showString("CONI");
        settingsStartTime = temperatureStartTime = millis();
    }

    delay(100);

    auto currentTime = millis();

    if (currentTime <= settingsStartTime ||
        currentTime <= temperatureStartTime)
    {
        //case when millis goes over 0 - once in rough 50 days
        settingsStartTime = temperatureStartTime = millis();
        return;
    }

    if (currentTime - settingsStartTime >= 500)
    {
        measurementDelay = getDelayFromSettings();
        settingsStartTime = currentTime;
    }

    //500 for measure time and 30 to send - so -530 ms
    if ((int)(currentTime - temperatureStartTime) >= measurementDelay - 530)
    {
        //last for 500 ms
        auto temperature = temperatureSensor.GetTemperature();
        showTemperature(temperature);
        sendTemperature(temperature);
        temperatureStartTime = currentTime;
    }
}
