#ifndef _THERMOMETER_LED_BRIGHTNESS_H_
#define _THERMOMETER_LED_BRIGHTNESS_H_

#include "IJson.h"

struct ThermometerLedBrightness : public IJson<ThermometerLedBrightness>
{
    int _brightness = 7;

    virtual nlohmann::json ToJson() const override
    {
        return {
            { "brightness", _brightness }
        };  
    }

    virtual void FromJson(const nlohmann::json& json) override
    {
        _brightness = json.value("brightness", 7);
    }
};

#endif //_THERMOMETER_LED_BRIGHTNESS_H_