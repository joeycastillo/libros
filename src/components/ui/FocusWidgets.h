#ifndef FocusWidgets_h
#define FocusWidgets_h

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include "Focus.h"

class Button : public Control {
public:
    Button(Rect rect, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
protected:
    std::string text;
};

class HatchedView : public View {
public:
    HatchedView(Rect rect, uint16_t color);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

class BorderedView : public View {
public:
    BorderedView(Rect rect) : View(rect) {};
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
};

class Label : public View {
public:
    Label(Rect rect, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
    void setText(std::string text);
protected:
    std::string text;
};

#endif // FocusWidgets_h