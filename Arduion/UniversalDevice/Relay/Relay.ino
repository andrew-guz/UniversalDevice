#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"
#include "RelayHelper.h"

RelayHelper relayHelper(26);
unsigned long settingsCommandStartTime;
unsigned long informStartTime;
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

//20ms
int getStateFromCommands()
{
    String url = String(API_COMMANDS) + String("/") + String(UUID);
    auto replyString = wifiHelper.GetRequest(url);
    DynamicJsonDocument doc(128);
    auto error = deserializeJson(doc, replyString);
    if (!error &&
        doc.containsKey("state"))
        return doc["state"].as<int>();
    return relayHelper.State(); //return what I have
}

JsonObject CurrentStateData()
{
    DynamicJsonDocument doc(128);
    auto root = doc.to<JsonObject>();
    root["state"] = relayHelper.State();
    return root;
}

//30 ms
void sendState()
{
    auto message = CreateMessage("relay", UUID, "relay_current_state", CurrentStateData());
    wifiHelper.PostRequestNoData(API_INFORM, message);
}

void setup()
{
    CheckDefines();

    Serial.begin(115200);
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
        settingsCommandStartTime = informStartTime = millis();
    }

    delay(100);

    auto currentTime = millis();

    if (currentTime <= settingsCommandStartTime ||
        currentTime <= informStartTime)
    {
        //case when millis goes over 0 - once in rough 50 days
        settingsCommandStartTime = informStartTime = millis();
        return;
    }

    if (currentTime - settingsCommandStartTime >= 500)
    {
        measurementDelay = getDelayFromSettings();
        auto stateFromCommand = getStateFromCommands();
        if (stateFromCommand != relayHelper.State())
        {
            stateFromCommand ?  relayHelper.On() : relayHelper.Off();
            sendState();
        }        
        settingsCommandStartTime = currentTime;
    }

    if (currentTime - informStartTime >= measurementDelay - 530)
    {
        //last for 500 ms
        sendState();
        informStartTime = currentTime;
    }
}