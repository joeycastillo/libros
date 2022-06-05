#include "OpenBookApplication.h"

std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);

void setup() {
    window = std::make_shared<Window>(300, 400);
    application = std::make_shared<OpenBookApplication>(window);
}

void loop() {
    application->run();
}
