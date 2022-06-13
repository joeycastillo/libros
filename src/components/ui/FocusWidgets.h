#ifndef FocusWidgets_h
#define FocusWidgets_h

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include "Focus.h"

class Button : public Control {
public:
    Button(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
protected:
    std::string text;
};

class HatchedView : public View {
public:
    HatchedView(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

class Label : public Control {
public:
    Label(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
    void setText(std::string text);
protected:
    std::string text;
};

#endif // FocusWidgets_h