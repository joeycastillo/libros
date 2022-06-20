#include "BookReaderViewController.h"

void BookReaderViewController::viewDidLoad() {
    ViewController::viewDidLoad();
}

void BookReaderViewController::viewWillAppear() {
    ViewController::viewWillAppear();
}

void BookReaderViewController::viewDidAppear() {
    ViewController::viewDidAppear();
}

void BookReaderViewController::viewWillDisappear() {
    ViewController::viewWillDisappear();
}

void BookReaderViewController::viewDidDisappear() {
    ViewController::viewDidDisappear();
}

void BookReaderViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<View>(MakeRect(0, 0, 300, 400));

    this->page = std::make_shared<Control>(MakeRect(0, 0, 300, 400));
    this->bookText = std::make_shared<OpenBookLabel>(MakeRect(6, 6, 300 - 12, 400 - 26), "");
    this->bookText->setWordWrap(true);
    this->bookText->setLineSpacing(2);
    this->bookText->setParagraphSpacing(8);
    this->page->addSubview(this->bookText);
    this->progressView = std::make_shared<ProgressView>(MakeRect(0, 400 - 8, 300, 8));
    this->progressView->setBackgroundColor(EPD_DARK);
    this->page->addSubview(this->progressView);

    // Actions for the book reading mode
    this->page->setAction(std::bind(&BookReaderViewController::returnHome, this, std::placeholders::_1), BUTTON_CENTER);
    this->page->setAction(std::bind(&BookReaderViewController::turnPage, this, std::placeholders::_1), BUTTON_PREV);
    this->page->setAction(std::bind(&BookReaderViewController::turnPage, this, std::placeholders::_1), BUTTON_NEXT);

}

void BookReaderViewController::turnPage(Event event) {
    switch (event.type) {
        case BUTTON_NEXT:
            // FIXME: check if this is the last page
            if (true) {
                this->currentPage++;
            }
            break;
        case BUTTON_PREV:
            if (this->currentPage > 0) {
                this->currentPage--;
            }
            break;
        default:
            return;
    }

    std::string text = OpenBookDatabase::sharedDatabase()->getBookPage(this->currentBook, this->currentPage);
    if (text[0] == 0x1e)this->bookText->setTextSize(2);
    else this->bookText->setTextSize(1);
    this->bookText->setText(text.c_str());
    // this->progressView->setProgress((float)(pos - textStart) / (float)(len - textStart));
}

void BookReaderViewController::returnHome(Event event) {
    // TODO: figure out a way to return home
}
