#include "OpenBookWidgets.h"
#include "OpenBookDevice.h"
#include <algorithm>

OpenBookButton::OpenBookButton(Rect rect, std::string text) : Button(rect, text) {
}

void OpenBookButton::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        View::draw(display, x, y);
        BabelTypesetterGFX *typesetter = OpenBookDevice::sharedDevice()->getTypesetter();
        typesetter->setWordWrap(false);
        typesetter->setBold(false);
        typesetter->setItalic(false);
        typesetter->setTextSize(1);
        typesetter->setParagraphSpacing(0);
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
    BabelTypesetterGFX *typesetter = OpenBookDevice::sharedDevice()->getTypesetter();
    typesetter->setLayoutArea(this->frame.origin.x + x, this->frame.origin.y + y, this->frame.size.width, this->frame.size.height);
    typesetter->setTextColor(this->foregroundColor);
    typesetter->setWordWrap(this->wrap);
    typesetter->setBold(this->bold);
    typesetter->setItalic(this->italic);
    typesetter->setTextSize(this->textSize);
    typesetter->setLineSpacing(this->lineSpacing);
    typesetter->setParagraphSpacing(this->paragraphSpacing);
    typesetter->print(this->text.c_str());
}

void OpenBookLabel::setWordWrap(bool value) {
    this->wrap = value;
    // TODO: setNeedsDisplay should be a method on View
    if (auto window = this->getWindow().lock()) {
        window->setNeedsDisplayInRect(this->frame, window);
    }
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

void OpenBookLabel::setLineSpacing(uint16_t value) {
    this->lineSpacing = value;
    if (auto window = this->getWindow().lock()) {
        window->setNeedsDisplayInRect(this->frame, window);
    }
}

void OpenBookLabel::setParagraphSpacing(uint16_t value) {
    this->paragraphSpacing = value;
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
        BabelTypesetterGFX *typesetter = OpenBookDevice::sharedDevice()->getTypesetter();
        typesetter->setBold(false);
        typesetter->setItalic(false);
        typesetter->setTextSize(1);
        typesetter->setParagraphSpacing(0);
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

bool OpenBookTable::becomeFocused() {
    if (this->selectedIndex >= 0 && this->selectedIndex < ((int32_t)this->subviews.size())) {
        return this->subviews[this->selectedIndex]->becomeFocused();
    }

    return Control::becomeFocused();
}

int32_t OpenBookTable::getSelectedIndex() {
    return this->selectedIndex;
}

bool OpenBookTable::handleEvent(Event event) {
    if (event.type == FOCUS_EVENT_BUTTON_CENTER) {
        // if user selected an item in the table, add that user info to the event
        event.userInfo = this->selectedIndex;
        return View::handleEvent(event);
    } else if (event.type < FOCUS_EVENT_BUTTON_CENTER) {
        // if the user moved the cursor around, first let the view select what to focus...
        bool retval = View::handleEvent(event);
        // then see if one of our views is focused; if so, update our index.
        this->selectedIndex = -1;
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
            if (std::shared_ptr<View> focusedView = window->getFocusedView().lock()) {
                this->selectedIndex = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
            }
        }
        // and return the value of View::handleEvent
        return retval;
    }

    // all other event types, just pass it along.
    return View::handleEvent(event);
}

OpenBookGrayscaleImageView::OpenBookGrayscaleImageView(Rect rect, const unsigned char *image) : View(rect) {
    this->image = image;
}

void OpenBookGrayscaleImageView::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    int16_t byteWidth = (this->frame.size.width * 2 + 7) / 8;
    uint8_t byte = 0;
    uint16_t color;

    for(int16_t j = 0; j < this->frame.size.height; j++) {
        for(int16_t i=0; i < this->frame.size.width * 2; i += 2 ) {
            if (i & 7){
                byte <<= 2;
            }
            else {
                byte = this->image[j * byteWidth + i / 8];
            }
            switch((byte & 0xC0) >> 6) {
                case 0:
                    color = EPD_BLACK;
                    break;
                case 1:
                    color = EPD_DARK;
                    break;
                case 2:
                    color = EPD_LIGHT;
                    break;
                case 3:
                    color = EPD_WHITE;
                    break;
                default:
                    break;
            }
            display->writePixel(x + this->frame.origin.x + i / 2, y + this->frame.origin.y + j, color);
        }
    }
}
