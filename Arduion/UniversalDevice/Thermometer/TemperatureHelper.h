#ifndef _TEMPERATURE_HELPER_H_
#define _TEMPERATURE_HELPER_H_

#include <OneWire.h>
#include <DallasTemperature.h>

class SingleTemperatureSensor
{
public:
    SingleTemperatureSensor(int pin) :
        _oneWire(pin),
        _sensor(&_oneWire)
    {      
    }

    void Setup()
    {
        _sensor.begin();
        _sensor.setResolution(12);
    }

    float GetTemperature()
    {
        _sensor.requestTemperatures();
        return _sensor.getTempCByIndex(0);
    }    

private:
    OneWire             _oneWire;
    DallasTemperature   _sensor;
};

#endif //_TEMPERATURE_HELPER_H_
