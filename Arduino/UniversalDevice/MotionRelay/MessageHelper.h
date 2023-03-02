#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include <ArduinoJson.h>

String CreateSimpleMessage(const String& type, const String& id, const String& subject, const String& name, float value)
{
    DynamicJsonDocument doc(512);
    auto root = doc.to<JsonObject>();
    auto header = root.createNestedObject("header");
    auto from = header.createNestedObject("description");
    from["type"] = type;
    from["id"] = id;
    header["subject"] = subject;
    auto data = root.createNestedObject("data");
    data[name] = value;
    String result;
    serializeJson(doc, result);
    return result;
}

String CreateSimpleMessage(const String& type, const String& id, const String& subject, const String& name, int value)
{
    DynamicJsonDocument doc(512);
    auto root = doc.to<JsonObject>();
    auto header = root.createNestedObject("header");
    auto from = header.createNestedObject("description");
    from["type"] = type;
    from["id"] = id;
    header["subject"] = subject;
    auto data = root.createNestedObject("data");
    data[name] = value;
    String result;
    serializeJson(doc, result);
    return result;
}

String CreateMessage(const String& type, const String& id, const String& subject, std::function<void(DynamicJsonDocument&)> func)
{
    DynamicJsonDocument doc(512);
    auto root = doc.to<JsonObject>();
    auto header = root.createNestedObject("header");
    auto from = header.createNestedObject("description");
    from["type"] = type;
    from["id"] = id;
    header["subject"] = subject;
    auto data = root.createNestedObject("data");
    func(doc);
    String result;
    serializeJson(doc, result);
    return result;
}

#endif //_MESSAGE_HELPER_H_
