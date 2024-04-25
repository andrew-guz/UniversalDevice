#pragma once

#include "Defines.hpp"
#include "IJson.hpp"

struct ThermometerLedBrightness : public IJson<ThermometerLedBrightness> {
    int _brightness = MAX_BRIGHTNESS;

    virtual nlohmann::json ToJson() const override { return { { "brightness", _brightness } }; }

    virtual void FromJson(const nlohmann::json& json) override { _brightness = json.value("brightness", 7); }
};
