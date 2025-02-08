#ifndef _DISPLAY_HELPER_H_
#define _DISPLAY_HELPER_H_

#include "Defines/Defines.h"

#ifdef TM1637_DISPLAY
#include <TM1637TinyDisplay.h>
#endif

#ifdef OLED_DISPLAY
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#endif

class Display {
protected:
    Display() = default;

public:
    virtual void Setup() = 0;

    virtual void SetBrightness(int value) = 0;

    void SetTemperature(float value) { _temperature = value; }

    void SetDateTime(const String& date, const String& time) {
        _date = date;
        _time = time;
    }

    enum class State {
        Hello,
        Connecting,
        Connected,
        Error,
        Temperature,
        Time,
        Date,
    };

    virtual void ShowState(State state) = 0;

protected:
    float _temperature;
    String _date;
    String _time;
};

#ifdef TM1637_DISPLAY
#define DISPLAY_MAX_BRIGHTNESS BRIGHT_7

class TM1637Display : public Display {
public:
    virtual void Setup() override {}

    void SetBrightness(int value) {
        if (value < BRIGHT_0)
            value = BRIGHT_0;
        if (value > BRIGHT_7)
            value = BRIGHT_7;
        if (value != BRIGHT_0)
            _display.setBrightness(value, true);
        else
            -display.setBrightness(value, false);
    }

    virtual void ShowState(State state) override {
        switch (state) {
            case State::Hello:
                _display.showString("HELO");
                break;
            case State::Connecting:
                _display.showString("CON-");
                break;
            case State::Connected:
                _display.showString("CONN");
                break;
            case State::Error:
                _display.showString("EROR");
                break;
            case State::Temperature:
            case State::Time:
            case State::Date:
                _display.showNumber(_temperature, 1);
                break;
        }
    }

private:
    TM1637TinyDisplay _display(LED_CLK_PIN, LED_DIO_PIN);
};
#endif // TM1637_DISPLAY

#ifdef OLED_DISPLAY
#define DISPLAY_MAX_BRIGHTNESS 1
#define SCREEN_WIDTH           128
#define SCREEN_HEIGHT          32
#define OLED_RESET             -1
#define SCREEN_ADDRESS         0x3C

class OLEDDisplay : public Display {
public:
    OLEDDisplay() :
        _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

    virtual void Setup() override {
        _display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.cp437(true);
    }

    virtual void SetBrightness(int value) override {
        if (value < 0)
            value = 0;
        if (value > 1)
            value = 1;
        _brightness = value;
        if (_brightness == 0)
            _display.clearDisplay();
    }

    virtual void ShowState(State state) override {
        switch (state) {
            case State::Hello:
                _display.setFont(&FreeSans18pt7b);
                ShowString("Hello");
                break;
            case State::Connecting:
                _display.setFont(&FreeSans12pt7b);
                ShowString("Connecting");
                break;
            case State::Connected:
                _display.setFont(&FreeSans12pt7b);
                ShowString("Connected");
                break;
            case State::Error:
                _display.setFont(&FreeSans18pt7b);
                ShowString("Error");
                break;
            case State::Temperature:
                _display.setFont(&FreeSans18pt7b);
                ShowString(String(_temperature, 1) + String(" C"));
                break;
            case State::Time:
                if (!_time.length()) {
                    _display.setFont(&FreeSans18pt7b);
                    ShowString(String(_temperature, 1) + String(" C"));
                } else {
                    _display.setFont(&FreeSans12pt7b);
                    ShowString(_time);
                }
                break;
            case State::Date:
                if (!_date.length()) {
                    _display.setFont(&FreeSans18pt7b);
                    ShowString(String(_temperature, 1) + String(" C"));
                } else {
                    _display.setFont(&FreeSans12pt7b);
                    ShowString(_date);
                }
                break;
        }
    }

protected:
    void ShowString(const String& str) {
        _display.clearDisplay();
        if (_brightness == 0) {
            _display.display();
            return;
        }
        int16_t x1, y1;
        uint16_t w, h;
        _display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
        _display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2 - y1);
        _display.println(str);
        _display.display();
    }

private:
    Adafruit_SSD1306 _display;
    int _brightness = 1;
};
#endif // OLED_DISPLAY

#endif // _DISPLAY_HELPER_H_
