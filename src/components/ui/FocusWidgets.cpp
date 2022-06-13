#include "FocusWidgets.h"
#include <algorithm>

Button::Button(Rect rect, std::string text) : Control(rect) {
    this->text = text;
}

void Button::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->getWindow().lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        display->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
        if (focusedView == this) {
            display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
            display->setTextColor(this->backgroundColor);
            display->print(this->text.c_str());
        } else {
            display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
            display->setTextColor(this->foregroundColor);
            display->print(this->text.c_str());
        }
    }
}

HatchedView::HatchedView(Rect rect, uint16_t color) : View(rect) {
    this->foregroundColor = color;
}

void HatchedView::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    for(int16_t i = x; i < x + this->frame.size.width; i++) {
        for(int16_t j = y; j < y + this->frame.size.height; j++) {
            if ((i + j) % 2) {
                display->drawPixel(i, j, this->foregroundColor);
            }
        }
    }
    View::draw(display, x, y);
}

void BorderedView::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
}

Label::Label(Rect rect, std::string text) : Control(rect) {
    this->text = text;
}

void Label::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    display->setTextColor(this->foregroundColor);
    display->setCursor(this->frame.origin.x + x, this->frame.origin.y + y);
    display->print(this->text.c_str());
}

void Label::setText(std::string text) {
    this->text = text;
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        window->setNeedsDisplay(true);
    }
}
