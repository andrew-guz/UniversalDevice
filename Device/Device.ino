#include "SetupHelper.h"
#include "UUID.h"
#include "core_esp8266_features.h"
#include <Arduino.h>
#include <algorithm>
#include <limits>
#include <map>
#ifdef WIFI_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#endif
#ifdef WIFI_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#endif
#include "MessageHelper.h"
#include <WebSocketsClient.h>
#ifdef HAS_THERMOMETER
#include "TemperatureHelper.h"
#endif
#ifdef HAS_LED
#include <TM1637TinyDisplay.h>
#endif
#ifdef HAS_RELAY
#include "RelayHelper.h"
#endif
#ifdef HAS_MOTION_RELAY
#include "MotionHelper.h"
#include "RelayHelper.h"
#endif

WebSocketsClient websocketClient;
std::map<String, unsigned long> ackMessages;
#ifdef HAS_THERMOMETER
SingleTemperatureSensor temperatureSensor(THERMOMETER_PIN);
unsigned long temperatureStartTime;
int temperatureMeasurementDelay = 5000;
#endif
#ifdef HAS_LED
TM1637TinyDisplay ledDisplay(LED_CLK_PIN, LED_DIO_PIN);
int ledBrightness = BRIGHT_7;
#endif
#ifdef HAS_RELAY
RelayHelper relayHelper(RELAY_PIN);
unsigned long relayStartTime;
int relayCheckStateDelay = 5000;
int relayStateFromCommand = 0;
#ifdef RELAY_AS_THERMOSTAT
SingleTemperatureSensor temperatureSensor(RELAY_THERMOSTAT_PIN);
#endif
#endif
#ifdef HAS_MOTION_RELAY
MotionHelper motionHelper(MOTION_RELAY_DETECTOR_PIN);
RelayHelper relayHelper(MOTION_RELAY_RELAY_PIN);
unsigned long relayStartTime;
int relayCheckStateDelay = 5000;
int motionActivityDelay = 60000;
int relayStateFromCommand = 0;
bool motionState = false;
unsigned long motionStartTime;
#endif

String generateMessageId() {
    UUID messageId;
    messageId.generate();
    String messageIdStr{ messageId.toCharArray() };
    ackMessages.insert(std::make_pair(messageIdStr, millis()));
    return messageIdStr;
}

#ifdef HAS_THERMOMETER
void sendTemperature(float temperature) {
    auto message = CreateSimpleMessage("thermometer", DEVICE_UUID, generateMessageId(), "thermometer_current_value", "value", temperature);
    websocketClient.sendTXT(message);
}
#endif

#ifdef HAS_LED
void ledSetBrightness(int value) {
    if (value != BRIGHT_0)
        ledDisplay.setBrightness(value, true);
    else
        ledDisplay.setBrightness(value, false);
}

void ledShowString(const String& str) { ledDisplay.showString(str.c_str()); }

void ledShowTemperature(float temperature) { ledDisplay.showNumber(temperature, 1); }
#endif

#ifdef HAS_RELAY
void sendRelayState() {
    auto message = CreateSimpleMessage("relay", DEVICE_UUID, generateMessageId(), "relay_current_state", "state", relayHelper.State());
    websocketClient.sendTXT(message);
}
#endif

#ifdef HAS_MOTION_RELAY
void sendMotionRelayState() {
    auto func = [](DynamicJsonDocument& doc) {
        doc["data"]["motion"] = motionState;
        doc["data"]["state"] = relayHelper.State();
    };
    auto message = CreateMessage("motion_relay", DEVICE_UUID, generateMessageId(), "motion_relay_current_state", func);
    websocketClient.sendTXT(message);
}

void setRelayState() {
    if (relayStateFromCommand == 1 || motionState == true)
        relayHelper.On();
    else
        relayHelper.Off();
}
#endif

void reconnectWebSocket() {
    websocketClient.disconnect();
    websocketClient.beginSSL(API_IP, API_PORT, API_WS);
}

bool connectToWiFi(const String& ssid, const String& password) {
    Serial.print("Connecting " + ssid + " ");
#ifdef HAS_LED
    ledShowString("CON-");
#endif
    WiFi.begin(ssid, password);
    for (auto i = 0; i < 30; ++i) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println(" Connected");
#ifdef HAS_LED
            ledShowString("CONN");
#endif
            return true;
        }
        Serial.print(".");
        delay(500);
    }
    Serial.println(" Failed");
#ifdef HAS_LED
    ledShowString("EROR");
#endif
    return false;
}

bool reconnectWiFi() {
    WiFi.disconnect();
    if (!connectToWiFi(SSID_MAIN, PASSWORD_MAIN))
        if (!connectToWiFi(SSID_ADDITIONAL, PASSWORD_ADDITIONAL))
            return false;
    reconnectWebSocket();
    return true;
}

bool checkWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        if (!connectToWiFi(SSID_MAIN, PASSWORD_MAIN))
            if (!connectToWiFi(SSID_ADDITIONAL, PASSWORD_ADDITIONAL))
                return false;
        reconnectWebSocket();
    }
    return true;
}

void authorizeAndGetSettings() {
    String type;
#ifdef HAS_THERMOMETER
    type = "thermometer";
#endif
#ifdef HAS_RELAY
    type = "relay";
#endif
#ifdef HAS_MOTION_RELAY
    type = "motion_relay";
#endif
    auto authMessage =
        CreateSimpleMessage(type, DEVICE_UUID, String(UUID{}.toCharArray()), "websocket_authorization", "authString", AUTHORIZATION_STR);
    websocketClient.sendTXT(authMessage);
    auto settingsMessage = CreateSimpleMessage(type, DEVICE_UUID, String(UUID{}.toCharArray()), "websocket_get_settings");
    websocketClient.sendTXT(settingsMessage);
    auto commandsMessage = CreateSimpleMessage(type, DEVICE_UUID, String(UUID{}.toCharArray()), "websocket_get_commands");
    websocketClient.sendTXT(commandsMessage);
}

char websocketBuffer[256];
bool websocketConnected = false;

void WebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_ERROR:
            Serial.println("Error");
            websocketConnected = false;
            break;
        case WStype_DISCONNECTED:
            Serial.println("Disconnected");
            websocketConnected = false;
            break;
        case WStype_CONNECTED: {
            Serial.println("Connected");
            websocketConnected = true;
            authorizeAndGetSettings();
        } break;
        case WStype_TEXT: {
            Serial.println("Incoming text");
            memset(websocketBuffer, 1, 256);
            sprintf(websocketBuffer, "%s", payload);
            StaticJsonDocument<256> doc;
            deserializeJson(doc, websocketBuffer);
            if (doc.containsKey("reauthorize"))
                authorizeAndGetSettings();
            if (doc.containsKey("acknowledge")) {
                String messageIdStr = doc["acknowledge"].as<String>();
                ackMessages.erase(messageIdStr);
            }
#ifdef HAS_THERMOMETER
            if (doc.containsKey("period"))
                temperatureMeasurementDelay = doc["period"].as<int>();
#ifdef HAS_LED
            if (doc.containsKey("brightness")) {
                ledBrightness = doc["brightness"].as<int>();
                ledSetBrightness(ledBrightness);
            }
#endif
#endif
#ifdef HAS_RELAY
            if (doc.containsKey("period"))
                relayCheckStateDelay = doc["period"].as<int>();
            if (doc.containsKey("state")) {
                relayStateFromCommand = doc["state"].as<int>();
                relayStateFromCommand ? relayHelper.On() : relayHelper.Off();
                sendRelayState();
            }
#endif
#ifdef HAS_MOTION_RELAY
            if (doc.containsKey("period"))
                relayCheckStateDelay = doc["period"].as<int>();
            if (doc.containsKey("activityTime"))
                motionActivityDelay = doc["activityTime"].as<int>();
            if (doc.containsKey("state")) {
                relayStateFromCommand = doc["state"].as<int>();
                setRelayState();
            }
#endif
        } break;
        case WStype_BIN:
            Serial.println("Incoming binary");
            break;
        case WStype_PING:
            Serial.println("Ping");
            break;
        case WStype_PONG:
            Serial.println("Pong");
            break;
    }
}

void setup() {
    CheckDefines();

    Serial.begin(115200);

#ifdef HAS_THERMOMETER
    temperatureSensor.Setup();
#endif

#ifdef HAS_LED
    ledSetBrightness(ledBrightness);
    ledShowString("HELO");
#endif

#ifdef HAS_RELAY
    relayHelper.Off();
#ifdef RELAY_AS_THERMOSTAT
    temperatureSensor.Setup();
#endif
#endif

#ifdef HAS_MOTION_RELAY
    relayHelper.Off();
    // sleep for 1 minute to be sure that motion sensor initialized
    Serial.println("Initializing motion sensor...");
    delay(60000);
    Serial.println("Motion sensor initializes...");
#endif

    websocketClient.beginSSL(API_IP, API_PORT, API_WS);
    websocketClient.enableHeartbeat(5000, 5000, 3);
    websocketClient.setReconnectInterval(500);
    websocketClient.onEvent(WebSocketEvent);
}

void loop() {
    // check the connection
    if (!checkWiFi()) {
#if defined HAS_RELAY && defined RELAY_AS_THERMOSTAT
        auto currentTemperature = temperatureSensor.GetTemperature();
        if (currentTemperature < RELAY_THERMOSTAT_VALUE - RELAY_THERMOSTAT_DELTA) {
            if (relayHelper.State() == 0)
                relayHelper.On();
        }
        if (currentTemperature > RELAY_THERMOSTAT_VALUE + RELAY_THERMOSTAT_DELTA) {
            if (relayHelper.State() == 1)
                relayHelper.Off();
        }
#endif
        delay(1000);
        return;
    }

    websocketClient.loop();

    auto currentTime = millis();

    // check that all data messages approved by ack
    static const unsigned long MAX_ACK_TIMEOUT = 5000;
    auto elapsedAck = std::find_if(ackMessages.begin(), ackMessages.end(), [currentTime](const std::pair<String, unsigned long>& kvp) -> bool {
        return currentTime > kvp.second ? currentTime - kvp.second >= MAX_ACK_TIMEOUT
                                        : (std::numeric_limits<unsigned long>().max() - kvp.second) + currentTime > MAX_ACK_TIMEOUT;
    });
    if (elapsedAck != ackMessages.end()) {
        Serial.println("Not ACK message found - reconnecting WiFi...");
        ackMessages.clear();
        if (!reconnectWiFi()) {
            delay(1000);
            return;
        }
    }

// case when millis goes over 0 - once in rough 50 days
#ifdef HAS_THERMOMTER
    if (currentTime <= temperatureStartTime) {
        temperatureStartTime = millis();
        return;
    }
#endif

#ifdef HAS_RELAY
    if (currentTime <= relayStartTime) {
        relayStartTime = millis();
        return;
    }
#endif

#ifdef HAS_MOTION_RELAY
    if (currentTime <= relayStartTime) {
        relayStartTime = millis();
        return;
    }
#endif

#ifdef HAS_THERMOMETER
    // 500 for measure time
    if ((int)(currentTime - temperatureStartTime) >= temperatureMeasurementDelay - 500) {
        // last for 500 ms
        auto temperature = temperatureSensor.GetTemperature();
#ifdef HAS_LED
        ledShowTemperature(temperature);
#endif
        sendTemperature(temperature);
        temperatureStartTime = currentTime;
    }
#endif

#ifdef HAS_RELAY
    if (currentTime - relayStartTime >= relayCheckStateDelay - 530) {
        // last for 500 ms
        sendRelayState();
        relayStartTime = currentTime;
    }
#ifdef RELAY_AS_THERMOSTAT
    if (websocketConnected == false) {
        auto currentTemperature = temperatureSensor.GetTemperature();
        if (currentTemperature < RELAY_THERMOSTAT_VALUE - RELAY_THERMOSTAT_DELTA) {
            if (relayHelper.State() == 0) {
                relayHelper.On();
                sendRelayState();
            }
        }
        if (currentTemperature > RELAY_THERMOSTAT_VALUE + RELAY_THERMOSTAT_DELTA) {
            if (relayHelper.State() == 1) {
                relayHelper.Off();
                sendRelayState();
            }
        }
    }
#endif
#endif

#ifdef HAS_MOTION_RELAY
    if (currentTime - relayStartTime >= relayCheckStateDelay) {
        sendMotionRelayState();
        relayStartTime = currentTime;
    }
    if (motionHelper.IsMotion()) {
        if (motionState == false) {
            motionState = true;
            setRelayState();
            sendMotionRelayState();
        }
        motionStartTime = currentTime;
    } else {
        if (motionState && currentTime - motionStartTime >= motionActivityDelay) {
            motionState = false;
            setRelayState();
            sendMotionRelayState();
        }
    }
#endif
}
