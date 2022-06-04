#include "Tasks.h"
#include "OpenBookApplication.h"
#include <memory>

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
    std::shared_ptr<Window> window = application->getWindow();
    if (window->needsDisplay()) {
        OpenBook_IL0398 *display = book->getDisplay();
        BabelTypesetterGFX *typesetter = book->getTypesetter();

        display->clearBuffer();
        window->draw(typesetter, 0, 0);

        Rect dirtyRect = window->getDirtyRect();

        if (RectsEqual(dirtyRect, window->getFrame())) {
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_QUICK);
            display->display();
        } else {
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_PARTIAL);
            display->displayPartial(dirtyRect.origin.x, dirtyRect.origin.y, dirtyRect.size.width, dirtyRect.size.height);
        }
        window->setNeedsDisplay(false);
    }

    return 0;
}

OpenBookLockScreen::OpenBookLockScreen(OpenBook *book) {
    this->book = book;
}

int16_t OpenBookLockScreen::run(Application *application) {
    OpenBookApplication *myApp = (OpenBookApplication *)application;
    if (myApp->locked) {
        std::shared_ptr<Window> window = application->getWindow();
        OpenBook_IL0398 *display = book->getDisplay();
        BabelTypesetterGFX *typesetter = book->getTypesetter();
        std::shared_ptr<Label> lockScreen = std::make_shared<Label>(32, 168, 300 - 64, 32, "Open Book is in low power mode.\nPress the lock button to wake.");
        window->addSubview(lockScreen);
        display->clearBuffer();
        window->draw(typesetter, 0, 0);

        display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_DEFAULT);
        display->display();
        myApp->book->lockDevice(); // we remain here in dormant mode until the lock button is pressed.
        window->removeSubview(lockScreen);
        window->setNeedsDisplay(true);
        myApp->locked = false;
    }

    return 0;
}
