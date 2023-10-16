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

  void Verbose(bool verbose)
  {
    _verbose = verbose;
  }

  float GetTemperature()
  {
    _sensor.requestTemperatures();
    auto temperature = _sensor.getTempCByIndex(0);        
    if (_verbose)
        PrintResults(temperature);
    return temperature;
  }

private:
  void PrintResults(float temperature)
  {
    Serial.print("Temp C: ");
    Serial.println(temperature);
  }

private:
  OneWire             _oneWire;
  DallasTemperature   _sensor;
  bool                _verbose = false;
};

#endif //_TEMPERATURE_HELPER_H_
