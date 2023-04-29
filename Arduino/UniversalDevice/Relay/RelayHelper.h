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
        #ifndef ON_LOW
            digitalWrite(_pin, HIGH);
        #else
            digitalWrite(_pin, LOW);
        #endif
    }

    void Off()
    {
        #ifndef ON_LOW
            digitalWrite(_pin, LOW);
        #else
            digitalWrite(_pin, HIGH);
        #endif
    }

    int State()
    {
        #ifndef ON_LOW
            return digitalRead(_pin);
        #else
            return !digitalRead(_pin);
        #endif
    }

private:
    int _pin;
};

#endif //_RELAY_HELPER_H_
