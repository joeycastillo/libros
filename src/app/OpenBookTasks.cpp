#include "OpenBookTasks.h"
#include "OpenBookApplication.h"
#include "OpenBookEvents.h"
#include <memory>

bool OpenBookRawButtonInput::run(Application *application) {
    OpenBookDevice *device = OpenBookDevice::sharedDevice();

    uint8_t buttons = device->readButtons();
    if (buttons && buttons != this->lastButtons) {
        this->lastButtons = buttons;
        if (buttons & OPENBOOK_BUTTONMASK_UP) {
            application->generateEvent(FOCUS_EVENT_BUTTON_UP, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_DOWN) {
            application->generateEvent(FOCUS_EVENT_BUTTON_DOWN, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_LEFT) {
            application->generateEvent(FOCUS_EVENT_BUTTON_LEFT, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_RIGHT) {
            application->generateEvent(FOCUS_EVENT_BUTTON_RIGHT, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_SELECT) {
            application->generateEvent(FOCUS_EVENT_BUTTON_TAP, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_PREVIOUS) {
            application->generateEvent(FOCUS_EVENT_BUTTON_PREV, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_NEXT) {
            application->generateEvent(FOCUS_EVENT_BUTTON_NEXT, 0);
        }
        if (buttons & OPENBOOK_BUTTONMASK_LOCK) {
            application->generateEvent(FOCUS_EVENT_BUTTON_LOCK, 0);
        }
    }

    if (!buttons) this->lastButtons = 0;

    return false;
}

bool OpenBookDisplay::run(Application *application) {
    OpenBookDevice *device = OpenBookDevice::sharedDevice();
    OpenBookApplication *myApp = (OpenBookApplication *)application;

    std::shared_ptr<Window> window = application->getWindow();
    if (window->needsDisplay()) {
        OpenBook_IL0398 *display = device->getDisplay();

        display->clearBuffer();
        window->draw(display, 0, 0);

        Rect dirtyRect = window->getDirtyRect();

        if (myApp->requestedRefreshMode != -1) {
            display->setDisplayMode((OpenBookDisplayMode)myApp->requestedRefreshMode);
            myApp->requestedRefreshMode = -1;
            display->display();
        } else if (RectsEqual(dirtyRect, window->getFrame())) {
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_QUICK);
            display->display();
        } else {
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_PARTIAL);
            display->displayPartial(dirtyRect.origin.x, dirtyRect.origin.y, dirtyRect.size.width, dirtyRect.size.height);
        }
        window->setNeedsDisplay(false);
    }

    return false;
}

bool OpenBookLockScreen::run(Application *application) {
    OpenBookDevice *device = OpenBookDevice::sharedDevice();

    OpenBookApplication *myApp = (OpenBookApplication *)application;
    if (myApp->locked) {
        std::shared_ptr<Window> window = application->getWindow();
        OpenBook_IL0398 *display = device->getDisplay();
        std::shared_ptr<BorderedView> lockModal = std::make_shared<BorderedView>(MakeRect(10, 168, 300 - 20, 68));
        std::shared_ptr<OpenBookLabel> lockLabel = std::make_shared<OpenBookLabel>(MakeRect(2, 2, 300 - 24, 64), "\n  Open Book is in low power mode.\n  Press the lock button to wake.");

        lockModal->addSubview(lockLabel);

        lockModal->setOpaque(true);
        lockModal->setBackgroundColor(EPD_WHITE);
        window->addSubview(lockModal);

        display->clearBuffer();
        window->draw(display, 0, 0);

        // this is pretty cool, if accidental: EPD_DARK renders as white with the
        // default or quick LUT, but dark in grayscale. so if we render this in
        // grayscale, it makes the white areas of the main screen appear dimmed,
        // as though we put a black layer with partial opacity.
        display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_GRAYSCALE);
        display->display();
        device->lockDevice(); // we remain here in dormant mode until the lock button is pressed.
        // at this time, the open book hardware resets when leaving low power mode, so the below code never runs.
        window->removeSubview(lockModal);
        window->setNeedsDisplay(true);
        myApp->locked = false;
    }

    return false;
}

bool OpenBookPowerMonitor::run(Application *application) {
    float systemVoltage = OpenBookDevice::sharedDevice()->getSystemVoltage();
    bool onBattery = systemVoltage < 4.5;

    if (onBattery != this->wasOnBattery) {
        application->generateEvent(OPEN_BOOK_EVENT_POWER_CHANGED, (int32_t) (systemVoltage * 100));
    }
    this->wasOnBattery = onBattery;

    return false;
}
