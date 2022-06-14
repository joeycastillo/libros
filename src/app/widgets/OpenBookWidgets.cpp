#include "OpenBookWidgets.h"
#include "OpenBookDevice.h"
#include <algorithm>

OpenBookButton::OpenBookButton(Rect rect, std::string text) : Button(rect, text) {
}

void OpenBookButton::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBookDevice::sharedInstance()->getTypesetter();
        typesetter->setCursor(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
        if (this->isFocused()) {
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
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBookDevice::sharedInstance()->getTypesetter();
        typesetter->setBold(false);
        typesetter->setItalic(false);
        Point cursorPosition = {0};
        uint16_t textColor = this->foregroundColor;
        switch (this->selectionStyle) {
            case CellSelectionStyleInvert:
                if (this->isFocused()) {
                    textColor = this->backgroundColor;
                }
                cursorPosition = MakePoint(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
                break;
            case CellSelectionStyleIndicatorLeading:
                cursorPosition = MakePoint(this->frame.origin.x + x + 16, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
                break;
            case CellSelectionStyleIndicatorTrailing:
                cursorPosition = MakePoint(this->frame.origin.x + x + 8, this->frame.origin.y + y + this->frame.size.height / 2 - 8);
                break;
            case CellSelectionStyleIndicatorAbove:
                cursorPosition = MakePoint(this->frame.origin.x + x, this->frame.origin.y + y + 5);
                break;
            case CellSelectionStyleIndicatorBelow:
                cursorPosition = MakePoint(this->frame.origin.x + x, this->frame.origin.y + y + this->frame.size.height - 16 - 5);
                break;
        }
        if (this->isFocused()) {
            Rect indicatorRect = this->_indicatorRect();
            typesetter->display->fillRect(x + indicatorRect.origin.x, y + indicatorRect.origin.y, indicatorRect.size.width, indicatorRect.size.height, this->foregroundColor);
        }
        typesetter->setTextColor(textColor);
        typesetter->setCursor(cursorPosition.x, cursorPosition.y);
        typesetter->print(this->text.c_str());
    }
}

void OpenBookCell::didBecomeFocused() {
    if (this->superview.lock()) {
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
            std::shared_ptr<View> shared_this = this->shared_from_this();
            window->setNeedsDisplayInRect(this->_indicatorRect(), shared_this);
        }
    }
}

void OpenBookCell::didResignFocus() {
    if (this->superview.lock()) {
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
            std::shared_ptr<View> shared_this = this->shared_from_this();
            window->setNeedsDisplayInRect(this->_indicatorRect(), shared_this);
        }
    }
}

Rect OpenBookCell::_indicatorRect() {
    switch (this->selectionStyle) {
        case CellSelectionStyleInvert:
            return MakeRect(this->frame.origin.x, this->frame.origin.y, this->frame.size.width, this->frame.size.height);
        case CellSelectionStyleIndicatorLeading:
            return MakeRect(this->frame.origin.x, this->frame.origin.y, 8, this->frame.size.height);
        case CellSelectionStyleIndicatorTrailing:
            return MakeRect(this->frame.origin.x + this->frame.size.width - 8, this->frame.origin.y, 8, this->frame.size.height);
        case CellSelectionStyleIndicatorAbove:
            return MakeRect(this->frame.origin.x, this->frame.origin.y, this->frame.size.width, 4);
        case CellSelectionStyleIndicatorBelow:
            return MakeRect(this->frame.origin.x, this->frame.origin.y + this->frame.size.height - 4, this->frame.size.width, 4);
    }

    return {0};
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
