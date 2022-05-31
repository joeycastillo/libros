#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"
#include "FocusWidgets.h"
#include <memory>

OpenBook *book;
std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);

void setup() {
    book = new OpenBook();
    window = std::make_shared<Window>(300, 400);
    application = std::make_shared<OpenBookApplication>(window, book);
}

void loop() {
    application->run();
}
