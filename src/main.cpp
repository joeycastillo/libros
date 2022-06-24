#include "OpenBookApplication.h"
#include "OpenBookDatabase.h"
#include "bitmaps.h"

std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);

void setup() {
    OpenBookDevice::sharedDevice()->startDisplay();

    // before initializing the window system, display the splash screen with a full refresh.
    Adafruit_EPD *display = OpenBookDevice::sharedDevice()->getDisplay();
    display->drawBitmap(0, 0, OpenBookSplash, 300, 400, EPD_BLACK);
    display->display();

    window = std::make_shared<Window>(MakeSize(300, 400));
    application = std::make_shared<OpenBookApplication>(window);
}

void loop() {
    application->run();
}
