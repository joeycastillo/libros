#include "Tasks.h"

OpenBookRawButtonInput::OpenBookRawButtonInput(OpenBook *book) {
    this->book = book;
}

int16_t OpenBookRawButtonInput::run(Application *application) {
    uint8_t buttons = book->readButtons();
    if (buttons && buttons != this->lastButtons) {
        this->lastButtons = buttons;
        if (buttons & OPENBOOK_BUTTONMASK_UP) {
            application->generateEvent(BUTTON_UP, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_DOWN) {
            application->generateEvent(BUTTON_DOWN, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_LEFT) {
            application->generateEvent(BUTTON_LEFT, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_RIGHT) {
            application->generateEvent(BUTTON_RIGHT, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_SELECT) {
            application->generateEvent(BUTTON_CENTER, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_PREVIOUS) {
            application->generateEvent(BUTTON_PREV, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_NEXT) {
            application->generateEvent(BUTTON_NEXT, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_LOCK) {
            application->generateEvent(BUTTON_LOCK, 0);
        }
    }

    if (!buttons) this->lastButtons = 0;

    return 0;
}

OpenBookDisplay::OpenBookDisplay(OpenBook *book) {
    this->book = book;
}

int16_t OpenBookDisplay::run(Application *application) {
    if (application->getWindow()->needsDisplay()) {
        OpenBook_IL0398 *display = book->getDisplay();
        Window *window = application->getWindow();

        display->clearBuffer();
        window->draw(display, 0, 0);

        Rect dirtyRect = window->getDirtyRect();

        Serial.print(dirtyRect.origin.x);
        Serial.print(" ");
        Serial.print(dirtyRect.origin.y);
        Serial.print(" ");
        Serial.print(dirtyRect.size.width);
        Serial.print(" ");
        Serial.print(dirtyRect.size.height);
        Serial.print(" ");
        if (RectsEqual(dirtyRect, window->getFrame())) {
            Serial.println("full update");
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_QUICK);
            display->display();
        } else {
            Serial.println("partial update");
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_PARTIAL);
            display->displayPartial(dirtyRect.origin.x, dirtyRect.origin.y, dirtyRect.size.width, dirtyRect.size.height);
        }
        application->getWindow()->setNeedsDisplay(false);
    }

    return 0;
}
