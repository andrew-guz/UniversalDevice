#include <assert.h>

#include "HTTPClient.h"

#include "SetupHelper.h"
#include "WiFiHelper.h"

bool WiFiConnected = false;

void setup()
{
    CheckDefines();

    Serial.begin(115200);
    WiFiConnected = WiFiConnect();
}

void loop()
{

}
