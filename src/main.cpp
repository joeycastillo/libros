#include "OpenBookApplication.h"
#include "OpenBookDatabase.h"

std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);

void setup() {
    OpenBookDevice::sharedDevice()->startDisplay();
    OpenBookDevice::sharedDevice()->startBabel();

    if (!OpenBookDevice::sharedDevice()->startSD()) while (true) Serial.println("No SD Card?");

    OpenBookDatabase::sharedDatabase()->connect();
    OpenBookDatabase::sharedDatabase()->scanForNewBooks();

    window = std::make_shared<Window>(MakeSize(300, 400));
    application = std::make_shared<OpenBookApplication>(window);
}

void loop() {
    application->run();
}
