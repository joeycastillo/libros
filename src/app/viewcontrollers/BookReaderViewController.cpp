#include "BookReaderViewController.h"
#include "OpenBookEvents.h"

BookReaderViewController::BookReaderViewController(BookRecord book) : ViewController() {
    this->book = book;
}

void BookReaderViewController::viewWillAppear() {
    ViewController::viewWillAppear();

    this->_updateView();
}

void BookReaderViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<View>(MakeRect(0, 0, 300, 400));

    this->bookText = std::make_shared<OpenBookLabel>(MakeRect(6, 6, 300 - 12, 400 - 26), "");
    this->bookText->setWordWrap(true);
    this->bookText->setLineSpacing(2);
    this->bookText->setParagraphSpacing(8);
    this->view->addSubview(this->bookText);
    this->progressView = std::make_shared<ProgressView>(MakeRect(0, 400 - 8, 300, 8));
    this->progressView->setBackgroundColor(EPD_DARK);
    this->view->addSubview(this->progressView);

    // This view is a little bit interesting: none of the above views are focusable!
    // That's because this isn't a menu or a dialog box; there's nothing to navigate,
    // therefore nothing to focus on.
    // So here we add an invisible control, just to receive events.
    this->eventReceiver = std::make_shared<Control>(MakeRect(0, 0, 0, 0));
    this->view->addSubview(this->eventReceiver);

    this->view->setAction(std::bind(&BookReaderViewController::returnHome, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_TAP);
    this->view->setAction(std::bind(&BookReaderViewController::turnPage, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_PREV);
    this->view->setAction(std::bind(&BookReaderViewController::turnPage, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_NEXT);
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
    this->generateEvent(OPEN_BOOK_EVENT_RETURN_HOME);
}

void BookReaderViewController::_updateView() {
    std::string text = OpenBookDatabase::sharedDatabase()->getBookPage(this->book, this->currentPage);
    if (text[0] == 0x1e)this->bookText->setTextSize(2);
    else this->bookText->setTextSize(1);
    this->bookText->setText(text.c_str());
    // this->progressView->setProgress((float)(pos - textStart) / (float)(len - textStart));
}
