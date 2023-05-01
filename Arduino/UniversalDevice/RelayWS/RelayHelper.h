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
        _state = 1;
    }

    void Off()
    {
        #ifndef ON_LOW
            digitalWrite(_pin, LOW);
        #else
            digitalWrite(_pin, HIGH);
        #endif
        _state = 0;
    }

    int State()
    {
        return _state;
    }

private:
    int _pin;
    int _state = 0;
};

#endif //_RELAY_HELPER_H_
