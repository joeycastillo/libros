#include "FocusWidgets.h"
#include <algorithm>

BitmapView::BitmapView(Rect rect, const unsigned char *bitmap) : View(rect) {
    this->bitmap = bitmap;
}

void BitmapView::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    display->drawBitmap(this->frame.origin.x, this->frame.origin.y, this->bitmap, this->frame.size.width, this->frame.size.height, this->foregroundColor);
}

Button::Button(Rect rect, std::string text) : Control(rect) {
    this->text = text;
}

void Button::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        display->setCursor(this->frame.origin.x + x + 4, this->frame.origin.y + y + this->frame.size.height / 2 - 4);
        if (this->focused) {
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

BorderedView::BorderedView(Rect rect) : View(rect) {
    this->opaque = true;
}

void BorderedView::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
}

void ProgressView::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->backgroundColor);
    display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, (int16_t)(this->frame.size.width * this->progress), this->frame.size.height, this->foregroundColor);
}

void ProgressView::setProgress(float value) {
    this->progress = value;
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        window->setNeedsDisplayInRect(this->frame, window);
    }
}

Label::Label(Rect rect, std::string text) : View(rect) {
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
        window->setNeedsDisplayInRect(this->frame, window);
    }
}
