#include "BookReaderViewController.h"
#include "OpenBookEvents.h"
#include <sstream>

BookReaderViewController::BookReaderViewController(std::shared_ptr<Application> application, BookRecord book) : ViewController(application) {
    this->book = book;
}

void BookReaderViewController::viewWillAppear() {
    ViewController::viewWillAppear();

    this->numPages = max(OpenBookDatabase::sharedDatabase()->numPages(this->book), 1);
    this->currentPage = OpenBookDatabase::sharedDatabase()->getCurrentPage(this->book);
    this->_updateView();
}

void BookReaderViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<Control>(MakeRect(0, 0, 300, 400));

    this->bookText = std::make_shared<OpenBookLabel>(MakeRect(6, 2, 300 - 12, 800 - 26), "");
    this->bookText->setWordWrap(true);
    this->bookText->setLineSpacing(2);
    this->bookText->setParagraphSpacing(8);
    this->view->addSubview(this->bookText);
    this->progressView = std::make_shared<ProgressView>(MakeRect(0, 400 - 4, 300, 4));
    this->view->addSubview(this->progressView);
    this->progressLabel = std::make_shared<Label>(MakeRect(0, 400 - 8, 90, 8), "");
    this->view->addSubview(this->progressLabel);

    this->view->setAction(std::bind(&BookReaderViewController::returnHome, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_TAP);
    this->view->setAction(std::bind(&BookReaderViewController::turnPage, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_PREV);
    this->view->setAction(std::bind(&BookReaderViewController::turnPage, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_NEXT);
    this->view->setAction(std::bind(&BookReaderViewController::showMenu, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_UP);
}

void BookReaderViewController::turnPage(Event event) {
    switch (event.type) {
        case FOCUS_EVENT_BUTTON_NEXT:
            // FIXME: check if this is the last page
            if (true) {
                this->currentPage++;
            }
            break;
        case FOCUS_EVENT_BUTTON_PREV:
            if (this->currentPage > 0) {
                this->currentPage--;
            }
            break;
        default:
            return;
    }

    this->_updateView();
}

void BookReaderViewController::returnHome(Event event) {
    OpenBookDatabase::sharedDatabase()->setCurrentPage(this->book, this->currentPage);
    this->generateEvent(OPEN_BOOK_EVENT_RETURN_HOME);
}

void BookReaderViewController::showMenu(Event event) {
    this->modal = std::make_shared<Control>(MakeRect(50, 200, 200, 200));

    std::shared_ptr<BorderedView> modalFrame = std::make_shared<BorderedView>(MakeRect(0, 0, 200, 201));
    modalFrame->setForegroundColor(EPD_BLACK);
    modalFrame->setBackgroundColor(EPD_WHITE);
    this->modal->addSubview(modalFrame);

    std::string bookTitle = OpenBookDatabase::sharedDatabase()->getBookTitle(this->book);
    std::string bookAuthor = OpenBookDatabase::sharedDatabase()->getBookAuthor(this->book);
    std::shared_ptr<OpenBookLabel> titleLabel = std::make_shared<OpenBookLabel>(MakeRect(4, 4, 200 - 8, 128), bookTitle.append(" by ").append(bookAuthor));
    titleLabel->setBold(true);
    titleLabel->setWordWrap(true);
    this->modal->addSubview(titleLabel);

    std::stringstream ss;
    ss << "Go to page " << this->currentPage + 1;
    this->gotoPageLabel = std::make_shared<OpenBookLabel>(MakeRect(4, 200 - 48, 200 - 8, 16), ss.str());
    this->gotoPageLabel->setItalic(true);
    this->modal->addSubview(this->gotoPageLabel);

    this->modalSlider = std::make_shared<ProgressView>(MakeRect(4, 200 - 24, 200 - 8, 16));
    this->modalSlider->setProgress(this->progressView->getProgress());
    this->modal->addSubview(this->modalSlider);

    this->view->addSubview(this->modal);

    this->modal->setAction(std::bind(&BookReaderViewController::handleModal, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_LEFT);
    this->modal->setAction(std::bind(&BookReaderViewController::handleModal, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_RIGHT);
    this->modal->setAction(std::bind(&BookReaderViewController::handleModal, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_TAP);

    this->modal->becomeFocused();
    this->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_GRAYSCALE);
}

void BookReaderViewController::handleModal(Event event) {
    float percentComplete;
    std::stringstream ss;
    switch (event.type) {
        case FOCUS_EVENT_BUTTON_LEFT:
            this->currentPage = max(this->currentPage - 10, 0);
            percentComplete = (float)(this->currentPage) / (float)(this->numPages);
            ss << "Go to page " << this->currentPage + 1;
            this->modalSlider->setProgress(percentComplete);
            this->gotoPageLabel->setText(ss.str());
            this->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_PARTIAL);
            break;
        case FOCUS_EVENT_BUTTON_RIGHT:
            this->currentPage = min(this->currentPage + 10, this->numPages);
            percentComplete = (float)(this->currentPage) / (float)(this->numPages);
            ss << "Go to page " << this->currentPage + 1;
            this->modalSlider->setProgress(percentComplete);
            this->gotoPageLabel->setText(ss.str());
            this->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_PARTIAL);
            break;
        case FOCUS_EVENT_BUTTON_TAP:
            this->view->removeSubview(this->modal);
            this->modal = NULL;
            this->modalSlider = NULL;
            this->gotoPageLabel = NULL;
            this->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_QUICK);
            _updateView();
            break;
    }
}

void BookReaderViewController::_updateView() {
    std::string text = OpenBookDatabase::sharedDatabase()->getTextForPage(this->book, this->currentPage);
    if (text[0] == 0x1e)this->bookText->setTextSize(2);
    else this->bookText->setTextSize(1);
    this->bookText->setText(text.c_str());

    float percentComplete = (float)(this->currentPage) / (float)(this->numPages);
    this->progressView->setProgress(percentComplete);

    char buf[16];
    sprintf(buf, "%ld/%ld", this->currentPage + 1, this->numPages);
    this->progressLabel->setText(buf);
    this->progressLabel->setForegroundColor(EPD_BLACK);
    // TODO: label should be able to align itself to the right, right?
    int16_t xpos = (int16_t)(percentComplete * 300) - strlen(buf) * 6;
    this->progressLabel->setFrame(MakeRect(max(0, xpos), 400 - 8 - 4, 90, 8));

    this->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_QUICK);
}
