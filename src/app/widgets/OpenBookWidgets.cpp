#include "OpenBookWidgets.h"
#include "OpenBookDevice.h"
#include <algorithm>

OpenBookButton::OpenBookButton(Rect rect, std::string text) : Button(rect, text) {
}

void OpenBookButton::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->getWindow().lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBookDevice::sharedInstance()->getTypesetter();
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

OpenBookLabel::OpenBookLabel(Rect rect, std::string text) : Label(rect, text) {
}

void OpenBookLabel::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    View::draw(display, x, y);
    BabelTypesetterGFX *typesetter = OpenBookDevice::sharedInstance()->getTypesetter();
    typesetter->setLayoutArea(this->frame.origin.x + x, this->frame.origin.y + y, this->frame.size.width, this->frame.size.height);
    typesetter->setTextColor(this->foregroundColor);
    typesetter->setBold(this->bold);
    typesetter->setItalic(this->italic);
    typesetter->setTextSize(this->textSize);
    typesetter->print(this->text.c_str());
}

void OpenBookLabel::setBold(bool value) {
    this->bold = value;
    // TODO: setNeedsDisplay should be a method on View
    if (auto window = this->getWindow().lock()) {
        window->setNeedsDisplayInRect(this->frame, window);
    }
}

void OpenBookLabel::setItalic(bool value) {
    this->italic = value;
    if (auto window = this->getWindow().lock()) {
        window->setNeedsDisplayInRect(this->frame, window);
    }
}

void OpenBookLabel::setTextSize(uint16_t value) {
    this->textSize = value;
    if (auto window = this->getWindow().lock()) {
        window->setNeedsDisplayInRect(this->frame, window);
    }
}

OpenBookCell::OpenBookCell(Rect rect, std::string text, OpenBookCellSelectionStyle selectionStyle) : Control(rect) {
    this->text = text;
    this->selectionStyle = selectionStyle;
}

void OpenBookCell::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // i hate this hack, window needs to manage views' state and communicate it down.
    View *focusedView = NULL;
    if (auto window = this->getWindow().lock()) {
        if (auto fv = window->getFocusedView().lock()) {
            focusedView = fv.get();
        }
    }
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBookDevice::sharedInstance()->getTypesetter();
        typesetter->setBold(false);
        typesetter->setItalic(false);
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

OpenBookTable::OpenBookTable(Rect rect, int16_t cellHeight, OpenBookCellSelectionStyle selectionStyle) : Control(rect) {
    this->selectionStyle = selectionStyle;
    this->cellHeight = cellHeight;
    this->cellsPerPage = rect.size.height / cellHeight;
}

void OpenBookTable::setItems(std::vector<std::string> items) {
    this->items = items;
    this->updateCells();
}

void OpenBookTable::updateCells() {
    this->items = items;
    this->subviews.clear();

    uint16_t end = this->startOffset + this->cellsPerPage;
    if (end > this->items.size()) end = this->items.size();
    std::vector<std::string>::iterator it;
    uint16_t i = 0;
    for(std::string text : this->items) {
        std::shared_ptr<OpenBookCell> cell = std::make_shared<OpenBookCell>(MakeRect(0, this->cellHeight * i++, this->frame.size.width, this->cellHeight), text, this->selectionStyle);
        this->addSubview(cell);
    }
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        window->setNeedsDisplay(true);
    }
}

bool OpenBookTable::handleEvent(Event event) {
    if (event.type == BUTTON_CENTER) {
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
            if (std::shared_ptr<View> focusedView = window->getFocusedView().lock()) {
                uint32_t index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
                event.userInfo = index;
            }
        }
    }

    return View::handleEvent(event);
}
