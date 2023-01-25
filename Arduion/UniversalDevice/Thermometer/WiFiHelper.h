#ifndef _WIFI_HELPER_H_
#define _WIFI_HELPER_H_

#include "WiFi.h"
#include "HTTPClient.h"

#include "Defines.h"


bool WiFiConnect(const String& ssid, const String& password)
{
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());
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

int PostJsonWithoutResults(const String& url, const String& data)
{
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(data);
    Serial.print("PostRequestWithoutResults - ");
    Serial.println(httpResponseCode);
    http.end();
}

#endif //_WIFI_HELPER_H_
