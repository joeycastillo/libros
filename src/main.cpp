#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"

OpenBook *book;
OpenBookApplication *application;
View *parent;

void setup() {
    book = new OpenBook();
    Window *window = new Window(300, 400);
    application = new OpenBookApplication(window, book);
    parent = new View(32, 32, 236, 336);
    window->addSubview(parent);

    View *child1 = new View(32, 32, 64, 64);
    View *child2 = new View(32, 128, 64, 64);
    View *child3 = new View(32, 224, 64, 64);
    parent->addSubview(child1);
    parent->addSubview(child2);
    parent->addSubview(child3);

    window->setFocusTargets(child1, NULL, NULL, child2, NULL);
    window->setFocusTargets(child2, child1, NULL, child3, NULL);
    window->setFocusTargets(child3, child2, NULL, NULL, NULL);

    child1->becomeFocused();

    application->run();
}

void loop() {
}
