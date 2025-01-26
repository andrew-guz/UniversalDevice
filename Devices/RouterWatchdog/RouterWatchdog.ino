// Setup relay - pin will be set to hight level that will break relay
#define RELAY_PIN

// Define network properties
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

#include <ESPping.h>

void setup() {
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
}

bool wifiExists() {
    Serial.println("Checking WiFi...");

    for (int i = 0; i < 30; ++i) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("WiFi connected.");

            // ya.ru
            static const IPAddress ip(8, 8, 8, 8);
            const bool ping = Ping.ping(ip) > 0;

            Serial.println("Ping result - " + String(ping ? "TRUE" : "FALSE") + ".");

            return ping;
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
        resetRelay();
        // if relay was resetted - sleep 5 seconds
        delay(5 * 60 * 1000);
    } else {
        // wifi is OK - sleep longer - 10 minutes
        delay(10 * 60 * 1000);
    }
}
