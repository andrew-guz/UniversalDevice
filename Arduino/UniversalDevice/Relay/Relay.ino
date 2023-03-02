#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"
#include "RelayHelper.h"

RelayHelper relayHelper(26);
unsigned long settingsCommandStartTime;
unsigned long informStartTime;
int informDelay = 5000;
int stateFromCommand = 0;

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
    return informDelay; //return what I remember
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
    return stateFromCommand; //return what I have
}

//30 ms
void sendState()
{
    auto message = CreateSimpleMessage("relay", UUID, "relay_current_state", "state", relayHelper.State());
    wifiHelper.PostRequestNoData(API_INFORM, message);
}

void setup()
{
    CheckDefines();

    Serial.begin(115200);

    relayHelper.Off();
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
        informDelay = getDelayFromSettings();
        stateFromCommand = getStateFromCommands();
        if (stateFromCommand != relayHelper.State())
        {
            stateFromCommand ?  relayHelper.On() : relayHelper.Off();
            sendState();
        }        
        settingsCommandStartTime = currentTime;
    }

    if (currentTime - informStartTime >= informDelay - 530)
    {
        //last for 500 ms
        sendState();
        informStartTime = currentTime;
    }
}