#include "BookListViewController.h"
#include "bitmaps.h"

void BookListViewController::viewDidLoad() {
    ViewController::viewDidLoad();

    this->view->setAction(std::bind(&BookListViewController::selectBook, this, std::placeholders::_1), BUTTON_CENTER);
}

void BookListViewController::viewWillAppear() {
    ViewController::viewWillAppear();

    // update books whenever the view appears
    std::vector<std::string> titles;
    uint32_t numBooks = OpenBookDatabase::sharedDatabase()->getNumberOfBooks();

    for (uint32_t i = 0 ; i < numBooks ; i++) {
        BookRecord record = OpenBookDatabase::sharedDatabase()->getBookRecord(i);
        std::string title = OpenBookDatabase::sharedDatabase()->getBookTitle(record);
        titles.push_back(title);
        this->books.push_back(record);
    }

    this->table->setItems(titles);
}

void BookListViewController::viewDidAppear() {
    ViewController::viewDidAppear();
}

void BookListViewController::viewWillDisappear() {
    ViewController::viewWillDisappear();
}

void BookListViewController::viewDidDisappear() {
    ViewController::viewDidDisappear();
}

void BookListViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<View>(MakeRect(0, 0, 300, 400));
    std::shared_ptr<OpenBookLabel> titleLabel = std::make_shared<OpenBookLabel>(MakeRect(28, 8, 200, 16), "My Library");
    std::shared_ptr<BitmapView> shelfIcon = std::make_shared<BitmapView>(MakeRect(9, 9, 16, 16), ShelfIcon);
    titleLabel->setBold(true);
    char buf[16];
    sprintf(buf, "VSYS: %4.2fV", OpenBookDevice::sharedDevice()->getSystemVoltage());
    std::shared_ptr<OpenBookLabel> batteryLabel = std::make_shared<OpenBookLabel>(MakeRect(200, 8, 80, 16), buf);
    this->table = std::make_shared<OpenBookTable>(MakeRect(0, 32, 300, 360), 24, CellSelectionStyleIndicatorLeading);
    this->view->addSubview(titleLabel);
    this->view->addSubview(shelfIcon);
    this->view->addSubview(batteryLabel);
    this->view->addSubview(this->table);
}

void BookListViewController::selectBook(Event event) {
    if (std::shared_ptr<Window>window = this->view->getWindow().lock()) {
        this->currentBook = this->books[event.userInfo];

        if (OpenBookDatabase::sharedDatabase()->bookIsPaginated(this->currentBook)) {
            // TODO: Somehow get application to present book reader
        } else {
            this->modal = std::make_shared<BorderedView>(MakeRect(20, 100, 300 - 20 * 2, 200));
            int16_t subviewWidth = this->modal->getFrame().size.width - 40;
            window->addSubview(this->modal);
            std::shared_ptr<OpenBookLabel> label = std::make_shared<OpenBookLabel>(MakeRect(20, 20, subviewWidth, 32), "This book is not paginated.\nPaginate it now?");
            this->modal->addSubview(label);
            std::shared_ptr<OpenBookButton> yes = std::make_shared<OpenBookButton>(MakeRect(20, 68, subviewWidth, 48), "Yes");
            yes->setAction(std::bind(&BookListViewController::paginate, this, std::placeholders::_1), BUTTON_CENTER);
            this->modal->addSubview(yes);
            std::shared_ptr<OpenBookButton> no = std::make_shared<OpenBookButton>(MakeRect(20, 132, subviewWidth, 48), "No");
            no->setAction(std::bind(&BookListViewController::dismiss, this, std::placeholders::_1), BUTTON_CENTER);
            this->modal->addSubview(no);
            this->modal->becomeFocused();
            // this->requestedRefreshMode = OPEN_BOOK_DISPLAY_MODE_GRAYSCALE;
        }
    }

}

void BookListViewController::dismiss(Event event) {
    if (std::shared_ptr<Window>window = this->view->getWindow().lock()) {
        window->removeSubview(this->modal);
        this->modal.reset();
        this->currentBook = {0};
    }
}

void BookListViewController::paginate(Event event) {
    if (std::shared_ptr<Window>window = this->view->getWindow().lock()) {
        window->removeSubview(this->modal);
        this->modal.reset();
        OpenBookDatabase::sharedDatabase()->paginateBook(this->currentBook);
    }
}
