#include "FocusWidgets.h"
#include <algorithm>

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

Cell::Cell(int16_t x, int16_t y, int16_t width, int16_t height, std::string text, CellSelectionStyle selectionStyle) : View(x, y, width, height) {
    this->text = text;
    this->selectionStyle = selectionStyle;
}

void Cell::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    display->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 4);
    // for now only implementing CellSelectionStyleInvert, just to get up and running
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
        Cell *cell = new Cell(0, this->cellHeight * i++, this->frame.size.width, this->cellHeight, text, this->selectionStyle);
        this->addSubview(cell);
    }
    this->window->setNeedsDisplay(true);
}

void Table::becomeFocused() {
    if (this->subviews.size()) {
        this->subviews.front()->becomeFocused();
    }
}

bool Table::handleEvent(Event event) {
    View *focusedView = this->window->getFocusedView();
    uint32_t index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
    switch (event.type) {
    case BUTTON_CENTER:
        // TODO: select the cell
        return true;
    case BUTTON_NEXT:
        // TODO: pagination
        return true;
    case BUTTON_PREV:
        // TODO: pagination
        return true;
    case BUTTON_UP:
        if (index > 0) this->subviews[index - 1]->becomeFocused();
        return true;
    case BUTTON_DOWN:
        if (index < this->subviews.size()- 1) this->subviews[index + 1]->becomeFocused();
        return true;
    default:
        return false;
    }
}
