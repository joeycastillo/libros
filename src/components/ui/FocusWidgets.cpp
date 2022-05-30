#include "FocusWidgets.h"

Button::Button(int16_t x, int16_t y, int16_t width, int16_t height, std::string text) : View(x, y, width, height) {
    this->text = text;
}

void Button::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    display->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 4);
    if (this->window->getFocusedView() == this) {
        display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, EPD_BLACK);
        display->setTextColor(EPD_WHITE);
        display->print(this->text.c_str());
    } else {
        display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, EPD_BLACK);
        display->setTextColor(EPD_BLACK);
        display->print(this->text.c_str());
    }
    View::draw(display, x, y);
}