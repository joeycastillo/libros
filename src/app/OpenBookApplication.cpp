#include "OpenBookApplication.h"
#include "OpenBookTasks.h"
#include "BookListViewController.h"
#include "BookReaderViewController.h"

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window) : Application(window) {
    // set up tasks for input, output and the lock screen
    std::shared_ptr<Task> lockScreenTask = std::make_shared<OpenBookLockScreen>();
    this->addTask(lockScreenTask);
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>();
    this->addTask(inputTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>();
    this->addTask(displayTask);

    std::shared_ptr<BookListViewController> bookList = std::make_shared<BookListViewController>();
    this->setRootViewController(bookList);

    // Actions for any mode
    this->window->setAction(std::bind(&OpenBookApplication::lockScreen, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_LOCK);
}

void OpenBookApplication::lockScreen(Event event) {
    this->locked = true;
}
