#include "OpenBookApplication.h"
#include "OpenBookTasks.h"
#include "Bitmaps.h"

// Callbacks
void selectBook(std::shared_ptr<Application>application, Event event);
void turnPage(std::shared_ptr<Application>application, Event event);
void returnHome(std::shared_ptr<Application>application, Event event);
void lockScreen(std::shared_ptr<Application>application, Event event);
void dismiss(std::shared_ptr<Application>application, Event event);
void paginate(std::shared_ptr<Application>application, Event event);

// Helpers
void updateBooks(OpenBookApplication *myApp);
void updatePage(std::shared_ptr<OpenBookApplication>myApp);

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window) : Application(window) {
    // set up tasks for input, output and the lock screen
    std::shared_ptr<Task> lockScreenTask = std::make_shared<OpenBookLockScreen>();
    this->addTask(lockScreenTask);
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>();
    this->addTask(inputTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>();
    this->addTask(displayTask);

    this->mainMenu = std::make_shared<View>(MakeRect(0, 0, 300, 400));
    std::shared_ptr<OpenBookLabel> titleLabel = std::make_shared<OpenBookLabel>(MakeRect(28, 8, 200, 16), "My Library");
    std::shared_ptr<BitmapView> shelfIcon = std::make_shared<BitmapView>(MakeRect(9, 9, 16, 16), ShelfIcon);
    titleLabel->setBold(true);
    char buf[16];
    sprintf(buf, "VSYS: %4.2fV", OpenBookDevice::sharedInstance()->getSystemVoltage());
    std::shared_ptr<OpenBookLabel> batteryLabel = std::make_shared<OpenBookLabel>(MakeRect(200, 8, 80, 16), buf);
    this->table = std::make_shared<OpenBookTable>(MakeRect(0, 32, 300, 360), 24, CellSelectionStyleIndicatorLeading);
    this->mainMenu->addSubview(titleLabel);
    this->mainMenu->addSubview(shelfIcon);
    this->mainMenu->addSubview(batteryLabel);
    this->mainMenu->addSubview(this->table);
    window->addSubview(this->mainMenu);
    updateBooks(this);

    this->page = std::make_shared<Control>(MakeRect(0, 0, 300, 400));
    this->bookText = std::make_shared<OpenBookLabel>(MakeRect(6, 6, 300 - 12, 400 - 26), "");
    this->bookText->setWordWrap(true);
    this->bookText->setLineSpacing(2);
    this->bookText->setParagraphSpacing(8);
    this->page->addSubview(this->bookText);
    this->progressView = std::make_shared<ProgressView>(MakeRect(0, 400 - 8, 300, 8));
    this->progressView->setBackgroundColor(EPD_DARK);
    this->page->addSubview(this->progressView);

    // Actions for the home menu
    this->mainMenu->setAction(&selectBook, BUTTON_CENTER);

    // Actions for the book reading mode
    this->page->setAction(&returnHome, BUTTON_CENTER);
    this->page->setAction(&turnPage, BUTTON_PREV);
    this->page->setAction(&turnPage, BUTTON_NEXT);

    // Actions for any mode
    this->window->setAction(&lockScreen, BUTTON_LOCK);
}

void selectBook(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();

    myApp->currentBook = myApp->books[event.userInfo];

    if (OpenBookDatabase::sharedInstance()->bookIsPaginated(myApp->currentBook)) {
        window->removeSubview(myApp->mainMenu);
        window->addSubview(myApp->page);
        myApp->page->becomeFocused();
        updatePage(myApp);
    } else {
        myApp->modal = std::make_shared<BorderedView>(MakeRect(20, 100, 300 - 20 * 2, 200));
        int16_t subviewWidth = myApp->modal->getFrame().size.width - 40;
        window->addSubview(myApp->modal);
        std::shared_ptr<OpenBookLabel> label = std::make_shared<OpenBookLabel>(MakeRect(20, 20, subviewWidth, 32), "This book is not paginated.\nPaginate it now?");
        myApp->modal->addSubview(label);
        std::shared_ptr<OpenBookButton> yes = std::make_shared<OpenBookButton>(MakeRect(20, 68, subviewWidth, 48), "Yes");
        yes->setAction(&paginate, BUTTON_CENTER);
        myApp->modal->addSubview(yes);
        std::shared_ptr<OpenBookButton> no = std::make_shared<OpenBookButton>(MakeRect(20, 132, subviewWidth, 48), "No");
        no->setAction(&dismiss, BUTTON_CENTER);
        myApp->modal->addSubview(no);
        myApp->modal->becomeFocused();
        myApp->requestedRefreshMode = OPEN_BOOK_DISPLAY_MODE_GRAYSCALE;
    }
}

void turnPage(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);

    switch (event.type) {
        case BUTTON_NEXT:
            // FIXME: check if this is the last page
            if (true) {
                myApp->currentPage++;
                updatePage(myApp);
            }
            break;
        case BUTTON_PREV:
            if (myApp->currentPage > 0) {
                myApp->currentPage--;
                updatePage(myApp);
            }
            break;
        default:
            break;
    }
}

void returnHome(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();

    window->removeSubview(myApp->page);
    window->addSubview(myApp->mainMenu);
    myApp->mainMenu->becomeFocused();
    window->setNeedsDisplay(true);
}

void lockScreen(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    myApp->locked = true;
}

void updateBooks(OpenBookApplication *myApp) {
    std::vector<std::string> titles;
    uint32_t numBooks = OpenBookDatabase::sharedInstance()->getNumberOfBooks();

    for (uint32_t i = 0 ; i < numBooks ; i++) {
        BookRecord record = OpenBookDatabase::sharedInstance()->getBookRecord(i);
        std::string title = OpenBookDatabase::sharedInstance()->getBookTitle(record);
        titles.push_back(title);
        myApp->books.push_back(record);
    }

    myApp->table->setItems(titles);
}

void updatePage(std::shared_ptr<OpenBookApplication>myApp) {
    std::string text = OpenBookDatabase::sharedInstance()->getBookPage(myApp->currentBook, myApp->currentPage);
    if (text[0] == 0x1e)myApp->bookText->setTextSize(2);
    else myApp->bookText->setTextSize(1);
    myApp->bookText->setText(text.c_str());
    // myApp->progressView->setProgress((float)(pos - textStart) / (float)(len - textStart));
}

void dismiss(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();
    window->removeSubview(myApp->modal);
    myApp->modal.reset();
    myApp->currentBook = {0};
}

void paginate(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();
    window->removeSubview(myApp->modal);
    myApp->modal.reset();
    OpenBookDatabase::sharedInstance()->paginateBook(myApp->currentBook);
}