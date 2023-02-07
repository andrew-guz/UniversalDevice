#include <assert.h>

#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"
#include "TemperatureHelper.h"

SingleTemperatureSensor temperatureSensor(26);
unsigned long time1;
int measurementDelay;

int getDelayFromSettings()
{
    String url = String(API_SETTINGS) + String("/") + String(UUID);
    auto replyString = GetRequest(url);
    DynamicJsonDocument doc(128);
    auto error = deserializeJson(doc, replyString);
    if (!error)
    {
        if (doc.containsKey("period"))
            return doc["period"].as<int>();
    }
    return 5000;
}

float getTemperature()
{
    auto temperature = temperatureSensor.GetTemperature();
    Serial.print("Temp C: ");
    Serial.println(temperature);
}

JsonObject CurrentValueData(float value)
{
    DynamicJsonDocument doc(128);
    auto root = doc.to<JsonObject>();
    root["value"] = value;
    return root;
}

void sendTemperature(float temperature)
{
    auto message = CreateMessage("thermometer", UUID, "current_value", CurrentValueData(temperature));
    PostRequestNoData(API_INFORM, message);
}

void setup()
{
    CheckDefines();

    Serial.begin(115200);

    temperatureSensor.Setup();
}

void loop()
{
    //check the connection
    if (WiFi.status() != WL_CONNECTED)
    {
        bool connected = WiFiConnect();
        if (!connected)
        {
            delay(1000);
            return;
        }
        time1 = millis();
    }

    delay(500);

    measurementDelay = getDelayFromSettings();

    auto time2 = millis();

    if (time2 <= time1)
    {
        //case when millis goes over 0 - once in rough 50 days
        time1 = millis();
        return;
    }

    if (time2 - time1 >= measurementDelay)
    {
        auto temperature = getTemperature();
        sendTemperature(temperature);
        time1 = millis();
    }
}
