#include "OpenBookApplication.h"

std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);

void setup() {
    // FIXME: the constructor does some hardware setup, so accessing it once makes sure it happens here first.
    // we should separate out the things that talk to the hardware into a begin() method.
    OpenBookDevice::sharedInstance();
    window = std::make_shared<Window>(MakeSize(300, 400));
    application = std::make_shared<OpenBookApplication>(window);
}

void loop() {
    application->run();
}
