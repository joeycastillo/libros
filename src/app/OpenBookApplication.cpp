#include "OpenBookApplication.h"
#include "OpenBookTasks.h"
#include "Bitmaps.h"

// Helpers
void updateBooks(OpenBookApplication *myApp);
void updatePage(OpenBookApplication *myApp);

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
    this->mainMenu->setAction(std::bind(&OpenBookApplication::selectBook, this, std::placeholders::_1), BUTTON_CENTER);

    // Actions for the book reading mode
    this->page->setAction(std::bind(&OpenBookApplication::returnHome, this, std::placeholders::_1), BUTTON_CENTER);
    this->page->setAction(std::bind(&OpenBookApplication::turnPage, this, std::placeholders::_1), BUTTON_PREV);
    this->page->setAction(std::bind(&OpenBookApplication::turnPage, this, std::placeholders::_1), BUTTON_NEXT);

    // Actions for any mode
    this->window->setAction(std::bind(&OpenBookApplication::lockScreen, this, std::placeholders::_1), BUTTON_LOCK);
}

void OpenBookApplication::selectBook(Event event) {
    std::shared_ptr<Window>window = this->getWindow();

    this->currentBook = this->books[event.userInfo];

    if (OpenBookDatabase::sharedInstance()->bookIsPaginated(this->currentBook)) {
        window->removeSubview(this->mainMenu);
        window->addSubview(this->page);
        this->page->becomeFocused();
        updatePage(this);
    } else {
        this->modal = std::make_shared<BorderedView>(MakeRect(20, 100, 300 - 20 * 2, 200));
        int16_t subviewWidth = this->modal->getFrame().size.width - 40;
        window->addSubview(this->modal);
        std::shared_ptr<OpenBookLabel> label = std::make_shared<OpenBookLabel>(MakeRect(20, 20, subviewWidth, 32), "This book is not paginated.\nPaginate it now?");
        this->modal->addSubview(label);
        std::shared_ptr<OpenBookButton> yes = std::make_shared<OpenBookButton>(MakeRect(20, 68, subviewWidth, 48), "Yes");
        yes->setAction(std::bind(&OpenBookApplication::paginate, this, std::placeholders::_1), BUTTON_CENTER);
        this->modal->addSubview(yes);
        std::shared_ptr<OpenBookButton> no = std::make_shared<OpenBookButton>(MakeRect(20, 132, subviewWidth, 48), "No");
        no->setAction(std::bind(&OpenBookApplication::dismiss, this, std::placeholders::_1), BUTTON_CENTER);
        this->modal->addSubview(no);
        this->modal->becomeFocused();
        this->requestedRefreshMode = OPEN_BOOK_DISPLAY_MODE_GRAYSCALE;
    }
}

void OpenBookApplication::turnPage(Event event) {
    switch (event.type) {
        case BUTTON_NEXT:
            // FIXME: check if this is the last page
            if (true) {
                this->currentPage++;
                updatePage(this);
            }
            break;
        case BUTTON_PREV:
            if (this->currentPage > 0) {
                this->currentPage--;
                updatePage(this);
            }
            break;
        default:
            break;
    }
}

void OpenBookApplication::returnHome(Event event) {
    std::shared_ptr<Window>window = this->getWindow();

    window->removeSubview(this->page);
    window->addSubview(this->mainMenu);
    this->mainMenu->becomeFocused();
    window->setNeedsDisplay(true);
}

void OpenBookApplication::lockScreen(Event event) {
    this->locked = true;
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

void updatePage(OpenBookApplication *myApp) {
    std::string text = OpenBookDatabase::sharedInstance()->getBookPage(myApp->currentBook, myApp->currentPage);
    if (text[0] == 0x1e)myApp->bookText->setTextSize(2);
    else myApp->bookText->setTextSize(1);
    myApp->bookText->setText(text.c_str());
    // myApp->progressView->setProgress((float)(pos - textStart) / (float)(len - textStart));
}

void OpenBookApplication::dismiss(Event event) {
    std::shared_ptr<Window>window = this->getWindow();
    window->removeSubview(this->modal);
    this->modal.reset();
    this->currentBook = {0};
}

void OpenBookApplication::paginate(Event event) {
    std::shared_ptr<Window>window = this->getWindow();
    window->removeSubview(this->modal);
    this->modal.reset();
    OpenBookDatabase::sharedInstance()->paginateBook(this->currentBook);
}