#include <tuple>

#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"
#include "MotionHelper.h"
#include "RelayHelper.h"

//d1 5
//d2 4
MotionHelper motionHelper(5);
RelayHelper relayHelper(4);
unsigned long settingsCommandStartTime;
unsigned long informStartTime;
int informDelay = 5000;
int activityDelay = 60000;
int stateFromCommand = 0;
bool motion = false;
unsigned long motionTime;

//20ms
std::tuple<int, int> getDelaysFromSettings()
{
    String url = String(API_SETTINGS) + String("/") + String(UUID);
    auto replyString = wifiHelper.GetRequest(url);
    DynamicJsonDocument doc(128);
    auto error = deserializeJson(doc, replyString);
    if (!error &&
        doc.containsKey("period") &&
        doc.containsKey("activityTime"))
    {
        auto p = doc["period"].as<int>();
        auto a = doc["activityTime"].as<int>();
        return std::make_pair(p, a);
    }
    return std::make_pair(informDelay, activityDelay); //return what I remember
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
    auto func = [](DynamicJsonDocument& doc)
    {        
        doc["data"]["motion"] = motion;
        doc["data"]["state"] = relayHelper.State();        
    };
    auto message = CreateMessage("motion_relay", UUID, "motion_relay_current_state", func);
    wifiHelper.PostRequestNoData(API_INFORM, message);
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

void setup()
{
    CheckDefines();

    Serial.begin(115200);

    relayHelper.Off();

    //sleep for 1 minute to be sure that motion sensor initialized
    delay(60000);
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
        std::tie(informDelay, activityDelay) = getDelaysFromSettings();
        stateFromCommand = getStateFromCommands();
        auto currentState = relayHelper.State();
        if (setRelayState() != currentState)
            sendState();
        settingsCommandStartTime = currentTime;
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
