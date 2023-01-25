#include <assert.h>

#include "SetupHelper.h"
#include "WiFiHelper.h"
#include "MessageHelper.h"

JsonObject CurrentValueData(float value)
{
    DynamicJsonDocument doc(128);
    auto root = doc.to<JsonObject>();
    root["value"] = value;
    return root;
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
        bool connected = WiFiConnect();
        if (!connected)
        {
            delay(1000);
            return;
        }
    }

    //send fake data
    auto message = CreateMessage("thermometer", UUID, "current_value", CurrentValueData(20.0f));
    PostJsonWithoutResults(API_INFORM, message);

    //sleep
    delay(5000);
}
