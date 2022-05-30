#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"
#include "FocusWidgets.h"

OpenBook *book;
OpenBookApplication *application;
View *parent;

void setup() {
    book = new OpenBook();
    Window *window = new Window(300, 400);
    application = new OpenBookApplication(window, book);
    parent = new View(32, 32, 236, 336);
    window->addSubview(parent);

    Button *child1 = new Button(32, 32, 128, 64, "Button 1");
    Button *child2 = new Button(32, 128, 128, 64, "Button 2");
    Button *child3 = new Button(32, 224, 128, 64, "Button 3");
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
