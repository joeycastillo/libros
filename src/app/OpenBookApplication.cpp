#include "OpenBookApplication.h"
#include "Tasks.h"

void centerButtonPressed(Event event) {
    Serial.println("center button pressed");
}

OpenBookApplication::OpenBookApplication(Window *window, OpenBook *book) : Application(window) {
    this->book = book;
    OpenBookRawButtonInput *inputTask = new OpenBookRawButtonInput(book);
    this->addTask(inputTask);
    window->setAction(&centerButtonPressed, BUTTON_CENTER);
}
