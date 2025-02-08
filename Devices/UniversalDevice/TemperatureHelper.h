#ifndef _TEMPERATURE_HELPER_H_
#define _TEMPERATURE_HELPER_H_

#include "Defines/Defines.h"

#ifdef ONE_WIRE_TEMPERATURE_SENSOR
#include <DallasTemperature.h>
#include <OneWire.h>
#endif

#ifdef I2C_TEMPERATURE_SENSOR
#include <Adafruit_BMP085.h>
#endif

class TemperatureSensor {
protected:
    TemperatureSensor() = default;

public:
    virtual ~TemperatureSensor() = default;

    virtual void Setup() = 0;

    void Verbose(bool verbose) { _verbose = verbose; }

    virtual float GetTemperature() = 0;

protected:
    void PrintResults(float temperature) {
        Serial.print("Temp C: ");
        Serial.println(temperature);
    }

    bool _verbose = false;
};

#ifdef ONE_WIRE_TEMPERATURE_SENSOR
class OneWireTemperatureSensor : public TemperatureSensor {
public:
    OneWireTemperatureSensor(int pin) :
        _oneWire(pin),
        _sensor(&_oneWire) {}

    virtual void Setup() override {
        _sensor.begin();
        _sensor.setResolution(12);
    }

    virtual float GetTemperature() override {
        _sensor.requestTemperatures();
        auto temperature = _sensor.getTempCByIndex(0);
        if (_verbose)
            PrintResults(temperature);
        return temperature;
    }

private:
    OneWire _oneWire;
    DallasTemperature _sensor;
};
#endif // ONE_WIRE_TEMPERATURE_SENSOR

#ifdef I2C_TEMPERATURE_SENSOR
class I2CTemperatureSensor : public TemperatureSensor {
public:
    I2CTemperatureSensor() = default;

    virtual void Setup() override { _sensor.begin(); }

    virtual float GetTemperature() override {
        auto temperature = _sensor.readTemperature();
        if (_verbose)
            PrintResults(temperature);
        return temperature;
    }

private:
    Adafruit_BMP085 _sensor;
};
#endif // I2C_TEMPERATURE_SENSOR

#endif //_TEMPERATURE_HELPER_H_
