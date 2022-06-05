#include "BabelWidgets.h"
#include "OpenBook.h"
#include <algorithm>

BabelButton::BabelButton(int16_t x, int16_t y, int16_t width, int16_t height, std::string text) : Button(x, y, width, height, text) {
}

void BabelButton::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->window.lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->window.lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBook::sharedInstance()->getTypesetter();
        typesetter->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
        if (focusedView == this) {
            typesetter->display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
            typesetter->setTextColor(this->backgroundColor);
            typesetter->print(this->text.c_str());
        } else {
            typesetter->display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
            typesetter->setTextColor(this->foregroundColor);
            typesetter->print(this->text.c_str());
        }
    }
}

BabelLabel::BabelLabel(int16_t x, int16_t y, int16_t width, int16_t height, std::string text) : Label(x, y, width, height, text) {
}

void BabelLabel::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    BabelTypesetterGFX *typesetter = OpenBook::sharedInstance()->getTypesetter();
    typesetter->setLayoutArea(this->frame.origin.x + x, this->frame.origin.y + y, this->frame.size.width, this->frame.size.height);
    typesetter->print(this->text.c_str());
}

BabelCell::BabelCell(int16_t x, int16_t y, int16_t width, int16_t height, std::string text, CellSelectionStyle selectionStyle) : View(x, y, width, height) {
    this->text = text;
    this->selectionStyle = selectionStyle;
}

void BabelCell::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->window.lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->window.lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBook::sharedInstance()->getTypesetter();
        typesetter->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
        // for now only implementing CellSelectionStyleInvert, just to get up and running
        if (focusedView == this) {
            typesetter->display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
            typesetter->setTextColor(this->backgroundColor);
            typesetter->print(this->text.c_str());
        } else {
            // typesetter->display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->foregroundColor);
            typesetter->setTextColor(this->foregroundColor);
            typesetter->print(this->text.c_str());
        }
    }
}

BabelTable::BabelTable(int16_t x, int16_t y, int16_t width, int16_t height, int16_t cellHeight, CellSelectionStyle selectionStyle) : View(x, y, width, height) {
    this->selectionStyle = selectionStyle;
    this->cellHeight = cellHeight;
    this->cellsPerPage = height / cellHeight;
}

void BabelTable::setItems(std::vector<std::string> items) {
    this->items = items;
    this->updateCells();
}

void BabelTable::updateCells() {
    this->items = items;
    this->subviews.clear();

    uint16_t end = this->startOffset + this->cellsPerPage;
    if (end > this->items.size()) end = this->items.size();
    std::vector<std::string>::iterator it;
    uint16_t i = 0;
    for(std::string text : this->items) {
        std::shared_ptr<BabelCell> cell = std::make_shared<BabelCell>(0, this->cellHeight * i++, this->frame.size.width, this->cellHeight, text, this->selectionStyle);
        this->addSubview(cell);
    }
    if (std::shared_ptr<Window> window = this->window.lock()) {
        window->setNeedsDisplay(true);
    }
}

void BabelTable::becomeFocused() {
    if (this->subviews.size()) {
        this->subviews.front()->becomeFocused();
    }
}

bool BabelTable::handleEvent(Event event) {
    if (event.type == BUTTON_CENTER) {
        if (std::shared_ptr<Window> window = this->window.lock()) {
            if (std::shared_ptr<View> focusedView = window->getFocusedView().lock()) {
                uint32_t index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
                event.userInfo = index;
            }
        }
    }

    return View::handleEvent(event);
}
