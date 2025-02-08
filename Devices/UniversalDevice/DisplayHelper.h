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

#include <Fonts/FreeSans18pt7b.h>
#endif

class Display {
protected:
    Display() = default;

    virtual void Setup() = 0;

    virtual void SetBrightness(int value) = 0;

    virtual void ShowString(const String& str) = 0;

    virtual void ShowTemperature(float value) = 0;
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

    virtual void ShowString(const String& str) override { _display.showString(str.c_str()); }

    virtual void ShowTemperature(float value) override { _display.showNumber(value, 1); }

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
        _display.setFont(&FreeSans18pt7b);
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

    virtual void ShowString(const String& str) override {
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

    virtual void ShowTemperature(float value) override { ShowString(String(value, 1) + String(" C")); }

private:
    Adafruit_SSD1306 _display;
    int _brightness = 1;
};
#endif // OLED_DISPLAY

#endif // _DISPLAY_HELPER_H_
