#include "OpenBookApplication.h"
#include "Tasks.h"

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book) : Application(window) {
    this->book = book;
    OpenBookRawButtonInput *inputTask = new OpenBookRawButtonInput(book);
    this->addTask(inputTask);
    OpenBookDisplay *displayTask = new OpenBookDisplay(book);
    this->addTask(displayTask);
}
