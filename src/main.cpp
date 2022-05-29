#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"

OpenBook *book;
OpenBookApplication *application;
View *parent;
View *child;

void toggleView(Event event) {
    if (child->getSuperview() == NULL) {
        parent->addSubview(child);
    } else {
        parent->removeSubview(child);
    }
}

void moveBox(Event event) {
    Rect frame = child->getFrame();
    switch (event.type) {
        case BUTTON_UP:
            frame.origin.y -= 16;
            break;
        case BUTTON_DOWN:
            frame.origin.y += 16;
            break;
        case BUTTON_LEFT:
            frame.origin.x += 16;
            break;
        case BUTTON_RIGHT:
            frame.origin.x -= 16;
            break;
        default:
            break;
    }
    child->setFrame(frame);
}

void setup() {
    book = new OpenBook();
    Window *window = new Window(300, 400);
    application = new OpenBookApplication(window, book);
    application->getWindow()->setAction(&toggleView, BUTTON_CENTER);
    application->getWindow()->setAction(&moveBox, BUTTON_UP);
    application->getWindow()->setAction(&moveBox, BUTTON_DOWN);
    application->getWindow()->setAction(&moveBox, BUTTON_LEFT);
    application->getWindow()->setAction(&moveBox, BUTTON_RIGHT);
    parent = new View(32, 32, 236, 336);
    child = new View(32, 32, 64, 64);
    window->addSubview(parent);

    application->run();
}

void loop() {
}
