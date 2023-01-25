#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include "ArduinoJson.h"

String CreateMessage(const String& type, const String& id, const String& subject, JsonObject data)
{
    DynamicJsonDocument doc(512);
    auto root = doc.to<JsonObject>();
    auto header = root.createNestedObject("header");
    auto deviceDescription = header.createNestedObject("deviceDescription");
    deviceDescription["type"] = type;
    deviceDescription["id"] = id;
    header["subject"] = subject;
    root["data"] = data;

    String result;
    serializeJson(doc, result);
    return result;
}

#endif //_MESSAGE_HELPER_H_
