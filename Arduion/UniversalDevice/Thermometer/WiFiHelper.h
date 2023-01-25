#ifndef _WIFI_HELPER_H_
#define _WIFI_HELPER_H_

#include "Defines.h"
#include "WiFi.h"

bool WiFiConnect(const char* ssid, const char* password)
{
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
        ++counter;
        if (counter > 100) //more then 10 seconds
        {
            Serial.println("Failed to connect.");
            return false;
        }
    }
    Serial.println("Connected");
    return true;
}

bool WiFiConnect()
{
    if (WiFiConnect(SSID_MAIN, PASSWORD_MAIN))
        return true;
    if (WiFiConnect(SSID_ADDITIONAL, PASSWORD_ADDITIONAL))
        return true;
    return false;
}

#endif //_WIFI_HELPER_H_
