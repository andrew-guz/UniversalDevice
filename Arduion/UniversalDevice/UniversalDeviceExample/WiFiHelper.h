#ifndef _WIFI_HELPER_H_
#define _WIFI_HELPER_H_

//set here headers suitable for WiFi and HttpClient
/*
#include <WiFi.h?
#include <HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
*/

#include "Defines.h"

class WiFiHelper
{
public:
    void Verbose(bool verbose)
    {
        _verbose = verbose;
    }

    bool WiFiConnect()
    {
        if (WiFiConnect(SSID_MAIN, PASSWORD_MAIN))
        {
            Recreate();
            return true;
        }
        if (WiFiConnect(SSID_ADDITIONAL, PASSWORD_ADDITIONAL))
        {
            Recreate();
            return true;
        }
        delete _wifiClient;
        delete _httpsClient;
        _wifiClient = nullptr;
        _httpsClient = nullptr;
        return false;
    }

    String GetRequest(const String& url)
    {
        String result;
        _httpsClient->begin(*_wifiClient, url);
        _httpsClient->addHeader("Authorization", AUTHORIZATION_STR);
        int httpResponseCode = _httpsClient->GET();
        if (_verbose)
            PrintResults("GetRequest", url, httpResponseCode);
        if (httpResponseCode > 0)
            result = _httpsClient->getString();
        else
            PrintResults("GetRequest", url, httpResponseCode);
        _httpsClient->end();
        return result;
    }

    int PostRequestNoData(const String& url, const String& data)
    {
        _httpsClient->begin(*_wifiClient, url);
        _httpsClient->addHeader("Content-Type", "application/json");
        _httpsClient->addHeader("Authorization", AUTHORIZATION_STR);
        int httpResponseCode = _httpsClient->POST(data);
        if (_verbose)
            PrintResults("PostRequestNoData", url, httpResponseCode);
        if (httpResponseCode < 0)
            PrintResults("PostRequestNoData", url, httpResponseCode);
        _httpsClient->end();
        return httpResponseCode;
    }

private:
    void Recreate()
    {
        delete _wifiClient;
        delete _httpsClient;
        _wifiClient = new WiFiClientSecure();
        _wifiClient->setInsecure();
        _httpsClient = new HTTPClient();
    }

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

    void PrintResults(const String& function, const String& url, int code)
    {
        Serial.print(function);
        Serial.print(" - ");
        Serial.print(url);
        Serial.print(" - ");
        Serial.println(code);
    }

private:
    WiFiClientSecure*   _wifiClient = nullptr;
    HTTPClient*         _httpsClient = nullptr;
    bool                _verbose = false;
};

WiFiHelper wifiHelper;

#endif //_WIFI_HELPER_H_
