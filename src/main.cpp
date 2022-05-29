#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"

OpenBook *book;
OpenBookApplication *application;
View *aView;

void toggleView(Event event) {
    if (aView->getSuperview() == NULL) {
        application->getWindow()->addSubview(aView);
    } else {
        application->getWindow()->removeSubview(aView);
    }
}

void moveBox(Event event) {
    switch (event.type) {
        case BUTTON_UP:
            aView->y -= 16;
            break;
        case BUTTON_DOWN:
            aView->y += 16;
            break;
        case BUTTON_LEFT:
            aView->x += 16;
            break;
        case BUTTON_RIGHT:
            aView->x -= 16;
            break;
        default:
            break;
    }
    application->getWindow()->setNeedsDisplay(true);
}

void setup() {
    book = new OpenBook();
    Window *window = new Window(0, 0, 300, 400);
    application = new OpenBookApplication(window, book);
    application->getWindow()->setAction(&toggleView, BUTTON_CENTER);
    application->getWindow()->setAction(&moveBox, BUTTON_UP);
    application->getWindow()->setAction(&moveBox, BUTTON_DOWN);
    application->getWindow()->setAction(&moveBox, BUTTON_LEFT);
    application->getWindow()->setAction(&moveBox, BUTTON_RIGHT);
    aView = new View(32, 32, 64, 64);

    application->run();
}

void loop() {
}
