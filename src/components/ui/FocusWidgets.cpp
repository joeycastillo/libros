#include "FocusWidgets.h"
#include <algorithm>

Button::Button(int16_t x, int16_t y, int16_t width, int16_t height, std::string text) : View(x, y, width, height) {
    this->text = text;
}

void Button::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->window.lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->window.lock()) {
        display->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 4);
        if (focusedView == this) {
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
}

Cell::Cell(int16_t x, int16_t y, int16_t width, int16_t height, std::string text, CellSelectionStyle selectionStyle) : View(x, y, width, height) {
    this->text = text;
    this->selectionStyle = selectionStyle;
}

void Cell::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->window.lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->window.lock()) {
        display->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 4);
        // for now only implementing CellSelectionStyleInvert, just to get up and running
        if (focusedView == this) {
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
}

Table::Table(int16_t x, int16_t y, int16_t width, int16_t height, int16_t cellHeight, CellSelectionStyle selectionStyle) : View(x, y, width, height) {
    this->selectionStyle = selectionStyle;
    this->cellHeight = cellHeight;
    this->cellsPerPage = height / cellHeight;
}

void Table::setItems(std::vector<std::string> items) {
    this->items = items;
    this->updateCells();
}

void Table::updateCells() {
    this->items = items;
    // TODO: free these
    this->subviews.clear();

    uint16_t end = this->startOffset + this->cellsPerPage;
    if (end > this->items.size()) end = this->items.size();
    std::vector<std::string>::iterator it;
    uint16_t i = 0;
    for(std::string text : this->items) {
        std::shared_ptr<Cell> cell = std::make_shared<Cell>(0, this->cellHeight * i++, this->frame.size.width, this->cellHeight, text, this->selectionStyle);
        this->addSubview(cell);
    }
    if (std::shared_ptr<Window> window = this->window.lock()) {
        window->setNeedsDisplay(true);
    }
}

void Table::becomeFocused() {
    if (this->subviews.size()) {
        this->subviews.front()->becomeFocused();
    }
}
