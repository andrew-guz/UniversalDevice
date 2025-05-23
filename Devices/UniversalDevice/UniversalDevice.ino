#include <Arduino.h>
#include <ArduinoOTA.h>
#include <algorithm>
#include <limits>
#include <map>

#include "Defines/Defines.h"
#include "SetupHelper.h"
#include "UUID.h"
#ifdef BOARD_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#endif // BOARD_ESP32
#ifdef BOARD_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#endif // BOARD_ESP8266
#include <WebSocketsClient.h>

#include "MessageHelper.h"
#ifdef HAS_THERMOMETER
#include "TemperatureHelper.h"
#ifdef HAS_DISPLAY
#include "DisplayHelper.h"
#endif // HAS_DISPLAY
#endif // HAS_THERMOMETER
#ifdef HAS_RELAY
#include "RelayHelper.h"
#ifdef RELAY_AS_THERMOSTAT
#include "TemperatureHelper.h"
#endif // RELAY_AS_THERMOSTAT
#endif // HAS_RELAY
#ifdef HAS_MOTION_RELAY
#include "MotionHelper.h"
#include "RelayHelper.h"
#endif // HAS_MOTION_RELAY

WebSocketsClient websocketClient;
std::map<String, unsigned long> ackMessages;
#ifdef HAS_THERMOMETER
#ifdef ONE_WIRE_TEMPERATURE_SENSOR
OneWireTemperatureSensor temperatureSensor(THERMOMETER_PIN);
#endif
#ifdef I2C_TEMPERATURE_SENSOR
I2CTemperatureSensor temperatureSensor;
#endif
unsigned long temperatureStartTime;
int temperatureMeasurementDelay = 5000;
#ifdef HAS_DISPLAY
unsigned long displayStartTime;
int displaySwitchDelay = 5000;
Display::State displayCurrentState = Display::State::Temperature;
#ifdef TM1637_DISPLAY
TM1637Display display(LED_CLK_PIN, LED_DIO_PIN);
#endif
#ifdef OLED_DISPLAY
OLEDDisplay display;
#endif
int displayBrightness = DISPLAY_MAX_BRIGHTNESS;
#endif // HAS_DISPLAY
#endif // HAS_THERMOMETER
#ifdef HAS_RELAY
RelayHelper relayHelper(RELAY_PIN);
unsigned long relayStartTime;
int relayCheckStateDelay = 5000;
int relayStateFromCommand = 0;
#ifdef RELAY_AS_THERMOSTAT
#ifdef ONE_WIRE_TEMPERATURE_SENSOR
OneWireTemperatureSensor temperatureSensor(RELAY_THERMOSTAT_PIN);
#endif
#ifdef I2C_TEMPERATURE_SENSOR
I2CTemperatureSensor temperatureSensor;
#endif
#endif // RELAY_AS_THERMOSTAT
#endif // HAS_RELAY
#ifdef HAS_MOTION_RELAY
MotionHelper motionHelper(MOTION_RELAY_DETECTOR_PIN);
RelayHelper relayHelper(MOTION_RELAY_RELAY_PIN);
unsigned long relayStartTime;
int relayCheckStateDelay = 5000;
int motionActivityDelay = 60000;
int relayStateFromCommand = 0;
bool motionState = false;
unsigned long motionStartTime;
#endif // HAS_MOTION_RELAY
#ifdef IS_UNIVERSAL
unsigned long universalDeviceStartTime;
int universalDeviceStateDelay = 5000;
#endif // IS_UNIVERSAL

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
#endif // HAS_THERMOMETER

#ifdef HAS_RELAY
void sendRelayState() {
    auto message = CreateSimpleMessage("relay", DEVICE_UUID, generateMessageId(), "relay_current_state", "state", relayHelper.State());
    websocketClient.sendTXT(message);
}
#endif // HAS_RELAY

#ifdef HAS_MOTION_RELAY
void sendMotionRelayState() {
    auto func = [](DynamicJsonDocument& doc) {
        doc["data"]["motion"] = motionState;
        doc["data"]["state"] = relayHelper.State();
    };
    auto message = CreateMessage("motion_relay", DEVICE_UUID, "motion_relay_current_state", generateMessageId(), func);
    websocketClient.sendTXT(message);
}

void setRelayState() {
    if (relayStateFromCommand == 1 || motionState == true)
        relayHelper.On();
    else
        relayHelper.Off();
}
#endif // HAS_MOTION_RELAY

#ifdef IS_UNIVERSAL
void sendUniversalState(JsonDocument& doc) {
    auto message =
        CreateSimpleMessage("universal_device", DEVICE_UUID, generateMessageId(), "universal_device_current_state", "values", doc.as<JsonObject>());
    websocketClient.sendTXT(message);
}
#endif // IS_UNIVERSAL

#if defined HAS_RELAY && defined RELAY_AS_THERMOSTAT
void thermostatActivity() {
    auto currentTemperature = temperatureSensor.GetTemperature();
    Serial.print("Themperature: ");
    Serial.println(currentTemperature);
    if (currentTemperature < -126.0f) {
        // broken sensor
        relayHelper.Off();
    } else {
        if (currentTemperature < RELAY_THERMOSTAT_VALUE - RELAY_THERMOSTAT_DELTA) {
            if (relayHelper.State() == 0) {
                relayHelper.On();
            }
        }
        if (currentTemperature > RELAY_THERMOSTAT_VALUE + RELAY_THERMOSTAT_DELTA) {
            if (relayHelper.State() == 1) {
                relayHelper.Off();
            }
        }
    }
}
#endif // #if defined HAS_RELAY && defined RELAY_AS_THERMOSTAT

void reconnectWebSocket() {
    websocketClient.disconnect();
    websocketClient.beginSSL(API_IP, API_PORT, API_WS);
}

bool connectToWiFi(const String& ssid, const String& password) {
    Serial.print("Connecting " + ssid + " ");
#ifdef HAS_DISPLAY
    display.ShowState(Display::State::Connecting);
#endif
    WiFi.begin(ssid, password);
    for (auto i = 0; i < 30; ++i) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println(" Connected");
#ifdef HAS_DISPLAY
            display.ShowState(Display::State::Connected);
            ;
#endif
            return true;
        }
        Serial.print(".");
        delay(500);
    }
    Serial.println(" Failed");
#ifdef HAS_DISPLAY
    display.ShowState(Display::State::Error);
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
#ifdef IS_UNIVERSAL
    type = "universal_device";
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
            if (doc.containsKey("restart")) {
                ESP.restart();
            }
#ifdef HAS_THERMOMETER
            if (doc.containsKey("period"))
                temperatureMeasurementDelay = doc["period"].as<int>();
#ifdef HAS_DISPLAY
            if (doc.containsKey("brightness")) {
                displayBrightness = doc["brightness"].as<int>();
                display.SetBrightness(displayBrightness);
            }
            if (doc.containsKey("date-time")) {
                int hour = doc["hour"].as<int>();
                int minute = doc["minute"].as<int>();
                int day = doc["day"].as<int>();
                int month = doc["month"].as<int>();
                int year = doc["year"].as<int>();
                display.SetDateTime(hour, minute, day, month, year);
            }
#endif // HAS_DISPLAY
#endif // HAS_THERMOMETER
#ifdef HAS_RELAY
            if (doc.containsKey("period"))
                relayCheckStateDelay = doc["period"].as<int>();
            if (doc.containsKey("state")) {
                relayStateFromCommand = doc["state"].as<int>();
                relayStateFromCommand ? relayHelper.On() : relayHelper.Off();
                sendRelayState();
            }
#endif // HAS_RELAY
#ifdef HAS_MOTION_RELAY
            if (doc.containsKey("period"))
                relayCheckStateDelay = doc["period"].as<int>();
            if (doc.containsKey("activityTime"))
                motionActivityDelay = doc["activityTime"].as<int>();
            if (doc.containsKey("state")) {
                relayStateFromCommand = doc["state"].as<int>();
                setRelayState();
            }
#endif // HAS_MOTION_RELAY
#ifdef IS_UNIVERSAL
            if (doc.containsKey("period"))
                universalDeviceStateDelay = doc["period"].as<int>();
#endif // IS_UNIVERSAL
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

#ifdef HAS_DISPLAY
    display.Setup();
    display.SetBrightness(displayBrightness);
    display.ShowState(Display::State::Hello);
#endif // HAS_DISPLAY
#endif // HAS_THERMOMETER

#ifdef HAS_RELAY
    relayHelper.Off();
#ifdef RELAY_AS_THERMOSTAT
    temperatureSensor.Setup();
#endif
#endif // HAS_RELAY

#ifdef HAS_MOTION_RELAY
    relayHelper.Off();
    // sleep for 1 minute to be sure that motion sensor initialized
    Serial.println("Initializing motion sensor...");
    delay(5000);
    Serial.println("Motion sensor initialized...");
#endif // HAS_MOTION_RELAY

    websocketClient.beginSSL(API_IP, API_PORT, API_WS);
    websocketClient.enableHeartbeat(5000, 5000, 3);
    websocketClient.setReconnectInterval(500);
    websocketClient.onEvent(WebSocketEvent);

    ArduinoOTA.setHostname(DEVICE_NAME);
    ArduinoOTA.setPasswordHash(OTA_PASSWORD);
    ArduinoOTA.begin();
}

void loop() {
    // check the connection
    if (!checkWiFi()) {
#if defined HAS_RELAY && defined RELAY_AS_THERMOSTAT
        thermostatActivity();
#endif // defined HAS_RELAY && defined RELAY_AS_THERMOSTAT
        delay(1000);
        return;
    }

    websocketClient.loop();

    ArduinoOTA.handle();

    auto currentTime = millis();

    // check that all data messages approved by ack
    static const unsigned long MAX_ACK_TIMEOUT = 30000;
    if (ackMessages.size()) {
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
    }

// case when millis goes over 0 - once in rough 50 days
#ifdef HAS_THERMOMTER
    if (currentTime <= temperatureStartTime) {
        temperatureStartTime = millis();
        return;
    }
#ifdef HAS_DISPLAY
    if (currentTime <= displayStartTime) {
        displayStartTime = millis();
        return;
    }
#endif // HAS_DISPLAY
#endif // HAS_THERMOMTER

#ifdef HAS_RELAY
    if (currentTime <= relayStartTime) {
        relayStartTime = millis();
        return;
    }
#endif // HAS_RELAY

#ifdef HAS_MOTION_RELAY
    if (currentTime <= relayStartTime) {
        relayStartTime = millis();
        return;
    }
#endif // HAS_MOTION_RELAY

#ifdef IS_UNIVERSAL
    if (currentTime <= universalDeviceStartTime) {
        universalDeviceStartTime = millis();
        return;
    }
#endif // IS_UNIVERSAL

#ifdef HAS_THERMOMETER
    // 500 for measure time
    if ((int)(currentTime - temperatureStartTime) >= temperatureMeasurementDelay - 500) {
        // last for 500 ms
        auto temperature = temperatureSensor.GetTemperature();
#ifdef HAS_DISPLAY
        display.SetTemperature(temperature);
#endif
        sendTemperature(temperature);
        temperatureStartTime = currentTime;
    }
#ifdef HAS_DISPLAY
    if ((int)(currentTime - displayStartTime) >= displaySwitchDelay) {
        display.ShowState(displayCurrentState);
        displayStartTime = currentTime;
        switch (displayCurrentState) {
            case Display::State::Temperature:
                displayCurrentState = Display::State::Time;
                break;
            case Display::State::Time:
                displayCurrentState = Display::State::Date;
                break;
            case Display::State::Date:
                displayCurrentState = Display::State::Temperature;
                break;
            default:
                break;
        }
    }
#endif // HAS_DISPLAY
#endif // HAS_THERMOMETER

#ifdef HAS_RELAY
    if (currentTime - relayStartTime >= relayCheckStateDelay - 530) {
        // last for 500 ms
        sendRelayState();
        relayStartTime = currentTime;
    }
#ifdef RELAY_AS_THERMOSTAT
    if (websocketConnected == false) {
        thermostatActivity();
    }
#endif // RELAY_AS_THERMOSTAT
#endif // HAS_RELAY

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
#endif // HAS_MOTION_RELAY

#ifdef IS_UNIVERSAL
    if (currentTime - universalDeviceStartTime >= universalDeviceStateDelay) {
        DynamicJsonDocument doc(512);
        // here - read sensors and prepare json
        // Format:
        // doc["put_property_name_here"]["type"] = "boolean/integer/double/string";
        // doc["put_property_name_here"]["value"] = put_value_here;
        sendUniversalState(doc);
        universalDeviceStartTime = currentTime;
    }
#endif // IS_UNIVERSAL
}
