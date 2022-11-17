#include "OpenBookTasks.h"
#include "OpenBookApplication.h"
#include "OpenBookEvents.h"
#include <memory>

bool OpenBookRawButtonInput::run(std::shared_ptr<Application> application) {
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

OpenBookDisplay::OpenBookDisplay() {
    OpenBookDevice *device = OpenBookDevice::sharedDevice();
    OpenBook_IL0398 *display = device->getDisplay();
    // when first initializing the display, clear once in quick mode to reduce ghosting.
    display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_QUICK);
    display->clearBuffer();
    display->display();
}

bool OpenBookDisplay::run(std::shared_ptr<Application> application) {
    OpenBookDevice *device = OpenBookDevice::sharedDevice();
    OpenBookApplication *myApp = (OpenBookApplication *)application.get();

    std::shared_ptr<Window> window = application->getWindow();
    if (window->needsDisplay()) {
        OpenBook_IL0398 *display = device->getDisplay();

        display->clearBuffer();
        window->draw(display, 0, 0);

        Rect dirtyRect = window->getDirtyRect();

        if (myApp->requestedRefreshMode == OPEN_BOOK_DISPLAY_MODE_DEFAULT || myApp->requestedRefreshMode == OPEN_BOOK_DISPLAY_MODE_QUICK || myApp->requestedRefreshMode == OPEN_BOOK_DISPLAY_MODE_GRAYSCALE) {
            display->setDisplayMode((OpenBookDisplayMode)myApp->requestedRefreshMode);
            myApp->requestedRefreshMode = -1;
            display->display();
        } else if (RectsEqual(dirtyRect, window->getFrame())) {
            display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_QUICK);
            display->display();
        } else {
            if (myApp->requestedRefreshMode == OPEN_BOOK_DISPLAY_MODE_FASTPARTIAL || myApp->requestedRefreshMode == OPEN_BOOK_DISPLAY_MODE_PARTIAL) {
                display->setDisplayMode((OpenBookDisplayMode)myApp->requestedRefreshMode);
                myApp->requestedRefreshMode = -1;
            } else {
                display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_PARTIAL);
            }
            display->displayPartial(dirtyRect.origin.x, dirtyRect.origin.y, dirtyRect.size.width, dirtyRect.size.height);
        }
        window->setNeedsDisplay(false);
    }

    return false;
}

bool OpenBookLockScreen::run(std::shared_ptr<Application> application) {
    OpenBookDevice *device = OpenBookDevice::sharedDevice();

    OpenBookApplication *myApp = (OpenBookApplication *)application.get();
    if (myApp->locked) {
        std::shared_ptr<Window> window = application->getWindow();
        OpenBook_IL0398 *display = device->getDisplay();
        std::shared_ptr<BorderedView> lockModal = std::make_shared<BorderedView>(MakeRect(-1, 400-32, 302, 33));
        std::shared_ptr<OpenBookLabel> lockLabel = std::make_shared<OpenBookLabel>(MakeRect(6, 8, 300 - 16, 16), "Slide the power switch to continue");
        std::shared_ptr<OpenBookLabel> arrowLabel = std::make_shared<OpenBookLabel>(MakeRect(300-18, 10, 16, 16), "➜");

        lockModal->addSubview(lockLabel);
        lockModal->addSubview(arrowLabel);

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

bool OpenBookPowerMonitor::run(std::shared_ptr<Application> application) {
    float systemVoltage = OpenBookDevice::sharedDevice()->getSystemVoltage();
    bool onBattery = systemVoltage < 4.5;

    if (onBattery != this->wasOnBattery) {
        application->generateEvent(OPEN_BOOK_EVENT_POWER_CHANGED, (int32_t) (systemVoltage * 100));
    }
    this->wasOnBattery = onBattery;

    return false;
}

bool BurnBabelImage::run(std::shared_ptr<Application> application) {
#ifdef ARDUINO_ARCH_RP2040
    static Adafruit_FlashTransport_SPI transport(1, SPI0);
    static Adafruit_SPIFlash flash(&transport);
    if (this->page == -1) {
        flash.begin();
        this->numPages = flash.numPages();
        flash.eraseChip();
        this->babelFile = OpenBookDevice::sharedDevice()->openFile("babel.bin");
    } else if (this->page < this->numPages) {
        uint8_t buffer[256];
        int16_t bytesRead = this->babelFile.read(buffer, 256);
        int16_t bytesWritten = flash.writeBuffer(this->page * 256, buffer, bytesRead);
        if (bytesRead != bytesWritten) {
            // TODO: Recover from this state?
            Serial.println("Flash error?");
        }
    } else {
        this->babelFile.close();
        OpenBookDevice::sharedDevice()->startBabel();
        OpenBookDatabase::sharedDatabase()->connect();
        OpenBookDatabase::sharedDatabase()->scanForNewBooks();

        std::shared_ptr<BookListViewController> mainViewController = std::make_shared<BookListViewController>(application);
        application->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_DEFAULT);
        application->setRootViewController(mainViewController);

        return true;
    }
    int32_t progress = (int32_t)(100 * (float)this->page / (float)this->numPages);
    if (progress != this->lastUpdate) {
        this->lastUpdate = progress;
        application->generateEvent(OPEN_BOOK_EVENT_PROGRESS, progress);
    }
    this->page++;

    return false;
#else
    return true;
#endif
}
