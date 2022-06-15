#include "OpenBookApplication.h"
#include "OpenBookDatabase.h"

std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);

void setup() {
    OpenBookDevice::sharedInstance()->startDisplay();
    OpenBookDevice::sharedInstance()->startBabel();

    if (!OpenBookDevice::sharedInstance()->startSD()) while (true) Serial.println("No SD Card?");

    OpenBookDatabase::sharedInstance()->connect();
    OpenBookDatabase::sharedInstance()->scanForNewBooks();

    window = std::make_shared<Window>(MakeSize(300, 400));
    application = std::make_shared<OpenBookApplication>(window);
}

void loop() {
    application->run();
}
