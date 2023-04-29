#ifndef _RELAY_HELPER_H_
#define _RELAY_HELPER_H_

class RelayHelper
{
public:
    RelayHelper(int pin) :
        _pin(pin)
    {
        pinMode(_pin, OUTPUT);
    }

    void On()
    {
        digitalWrite(_pin, HIGH);
    }

    void Off()
    {
        digitalWrite(_pin, LOW);
    }

    int State()
    {
        return digitalRead(_pin);
    }

private:
    int _pin;
};

#endif //_RELAY_HELPER_H_
