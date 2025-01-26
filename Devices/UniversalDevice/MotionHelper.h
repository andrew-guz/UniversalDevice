#ifndef _MOTION_HELPER_H_
#define _MOTION_HELPER_H_

class MotionHelper {
public:
    MotionHelper(int pin) : _pin(pin) { pinMode(_pin, INPUT); }

    bool IsMotion() { return digitalRead(_pin) == HIGH; }

private:
    int _pin;
};

#endif //_MOTION_HELPER_H_
