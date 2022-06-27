#include "OpenBookApplication.h"
#include "OpenBookTasks.h"
#include "OpenBookEvents.h"
#include "BookReaderViewController.h"
#include "FatalErrorViewController.h"

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window) : Application(window) {
    // set up tasks for input, output and the lock screen
    std::shared_ptr<Task> lockScreenTask = std::make_shared<OpenBookLockScreen>();
    this->addTask(lockScreenTask);
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>();
    this->addTask(inputTask);
    std::shared_ptr<Task> powerTask = std::make_shared<OpenBookPowerMonitor>();
    this->addTask(powerTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>();
    this->addTask(displayTask);

    bool ready = true;
    if (!OpenBookDevice::sharedDevice()->startSD()) {
        this->requestedRefreshMode = OPEN_BOOK_DISPLAY_MODE_DEFAULT;
        std::shared_ptr<FatalErrorViewController> modal = std::make_shared<FatalErrorViewController>("Please insert a MicroSD card.");
        this->setRootViewController(modal);
        ready = false;
    }
    if (!OpenBookDevice::sharedDevice()->startBabel()) {
        this->requestedRefreshMode = OPEN_BOOK_DISPLAY_MODE_DEFAULT;
        std::shared_ptr<FatalErrorViewController> modal = std::make_shared<FatalErrorViewController>("Language chip not initialized.");
        this->setRootViewController(modal);
        ready = false;
    }

    if (ready) {
        OpenBookDatabase::sharedDatabase()->connect();
        OpenBookDatabase::sharedDatabase()->scanForNewBooks();

        this->mainMenu = std::make_shared<BookListViewController>();
        this->setRootViewController(this->mainMenu);
    }

    this->window->setAction(std::bind(&OpenBookApplication::showLockScreen, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_LOCK);
    this->window->setAction(std::bind(&OpenBookApplication::showBookReader, this, std::placeholders::_1), OPEN_BOOK_EVENT_BOOK_SELECTED);
    this->window->setAction(std::bind(&OpenBookApplication::returnHome, this, std::placeholders::_1), OPEN_BOOK_EVENT_RETURN_HOME);
    this->window->setAction(std::bind(&OpenBookApplication::changeRefreshMode, this, std::placeholders::_1), OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE);
}

void OpenBookApplication::showLockScreen(Event event) {
    this->locked = true;
}

void OpenBookApplication::showBookReader(Event event) {
    BookRecord book = *(BookRecord *)event.userInfo;
    std::shared_ptr<BookReaderViewController> nextViewController = std::make_shared<BookReaderViewController>(book);
    this->setRootViewController(nextViewController);
}

void OpenBookApplication::returnHome(Event event) {
    this->setRootViewController(this->mainMenu);
}

void OpenBookApplication::changeRefreshMode(Event event) {
    this->requestedRefreshMode = event.userInfo;
}
