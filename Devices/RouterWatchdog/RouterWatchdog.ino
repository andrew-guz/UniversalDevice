#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>

// Setup relay - pin will be set to hight level that will break relay
#define RELAY_PIN

// Define network properties
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""
#define CHECK_URL     ""

int failCount = 0;

void setup() {
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    pinMode(LED_BUILTIN, OUTPUT);
    // beware - ESP built in led is off by HIGHT level
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
}

bool wifiExists() {
    Serial.println("Checking WiFi...");

    for (int i = 0; i < 30; ++i) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi connected.");

            WiFiClientSecure client;
            client.setInsecure();
            HTTPClient https;
            const String url = "https://" + String(CHECK_URL) + "/api/version";
            https.begin(client, url);
            const int getResult = https.GET();
            Serial.print("Get result: ");
            Serial.println(getResult);
            if (getResult == 200) {
                Serial.println("Server is available.");
                return true;
            } else {
                Serial.println("Server is not available.");
                return false;
            }
        }
        delay(500);
    }

    Serial.println("WiFi connection failed!");

    return false;
}

void resetRelay() {
    Serial.println("Relay reset started...");

    digitalWrite(RELAY_PIN, HIGH);
    delay(10 * 1000);
    digitalWrite(RELAY_PIN, LOW);

    Serial.println("Relay reset finished...");
}

void loop() {
    if (!wifiExists()) {
        digitalWrite(LED_BUILTIN, LOW);
        ++failCount;
        if (failCount > 3) {
            resetRelay();
            failCount = 0;
        }
        // if wi fi has problems - sleep only 5 minutes
        delay(5 * 60 * 1000);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
        // wifi is OK - sleep longer - 10 minutes
        delay(10 * 60 * 1000);
    }
}
