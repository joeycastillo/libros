#include "OpenBookApplication.h"
#include "Tasks.h"

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book) : Application(window) {
    this->book = book;
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>(book);
    this->addTask(inputTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>(book);
    this->addTask(displayTask);
}
