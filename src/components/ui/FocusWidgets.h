#ifndef FocusWidgets_h
#define FocusWidgets_h

#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include "Focus.h"
#include "Adafruit_EPD.h"

class Button : public View {
public:
    Button(int16_t x, int16_t y, int16_t width, int16_t height, std::string text);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y) override;
protected:
    std::string text;
};

#endif // FocusWidgets_h