#include "OpenBookApplication.h"
#include "Tasks.h"
#include "Bitmaps.h"

// Callbacks
void selectBook(std::shared_ptr<Application>application, Event event);
void turnPage(std::shared_ptr<Application>application, Event event);
void returnHome(std::shared_ptr<Application>application, Event event);
void lockScreen(std::shared_ptr<Application>application, Event event);

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
    this->bookText = std::make_shared<OpenBookLabel>(MakeRect(16, 16, 300 - 32, 400 - 32), "");
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
    OpenBookDevice *device = OpenBookDevice::sharedInstance();

    int32_t selectedIndex = event.userInfo;
    if (selectedIndex >= 0 && selectedIndex < (int32_t)myApp->filenames.size()) {
        myApp->currentBook = myApp->filenames[selectedIndex];
        std::string currentProgressFile = myApp->filenames[selectedIndex];
        currentProgressFile[currentProgressFile.size() - 1] = 'p';
        if (device->getSD()->exists(currentProgressFile.c_str())) {
            File f = device->getSD()->open(currentProgressFile.c_str(), FILE_READ);
            f.read(&myApp->currentLine, sizeof(size_t));
            f.close();
        } else {
            myApp->currentLine = 2; // start of lines, after title and author
            File f = device->getSD()->open(currentProgressFile.c_str(), FILE_WRITE);
            f.write((byte *)&(myApp->currentLine), sizeof(size_t));
            f.close();
        }

        window->removeSubview(myApp->mainMenu);
        window->addSubview(myApp->page);
        myApp->page->becomeFocused();
        updatePage(myApp);
    }
}

void turnPage(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();
    OpenBookDevice *device = OpenBookDevice::sharedInstance();

    switch (event.type) {
        case BUTTON_NEXT:
            if (!myApp->bookAtEnd) {
                myApp->currentLine += 22;
                myApp->bookNeedsRefresh = true;
            }
            break;
        case BUTTON_PREV:
            myApp->currentLine -= 22;
            myApp->bookAtEnd = false;
            myApp->bookNeedsRefresh = true;
            break;
        default:
            break;
    }
    std::string currentProgressFile = myApp->currentBook;
    currentProgressFile[currentProgressFile.size() - 1] = 'p';
    File f = device->getSD()->open(currentProgressFile.c_str(), FILE_READ);
    f.write((byte *)&(myApp->currentLine), sizeof(size_t));
    f.close();

    updatePage(myApp);
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
    OpenBookDevice *device = OpenBookDevice::sharedInstance();

    File root = device->getSD()->open("/");
    File entry = root.openNextFile();
    while (entry) {
        if (!entry.isDirectory()) {
            uint64_t magic = 0;
            entry.read((void *)&magic, 8);
            if (magic == 5426643222204338255) { // the string "OPENBOOK"
                char *filename = (char *)malloc(128);
                entry.getName(filename, 128);
                File file = device->getSD()->open(filename);
                file.seek(8);
                uint64_t title_loc;
                uint32_t title_len;
                file.read(&title_loc, 8);
                file.read(&title_len, 4);
                char *title = (char *)malloc(title_len + 1);
                file.seek(title_loc);
                file.read(title, title_len);
                title[title_len] = 0;
                titles.push_back(std::string(title));
                myApp->filenames.push_back(std::string(filename));
                free(title);
            }
        }
        entry = root.openNextFile();
    }

    myApp->table->setItems(titles);
}

void updatePage(std::shared_ptr<OpenBookApplication>myApp) {
    OpenBookDevice *device = OpenBookDevice::sharedInstance();
    std::string pageText = "";
    FatFile file = device->getSD()->open(myApp->currentBook.c_str(), FILE_READ);

    for(int i = 0; i < 22; i++) {
        uint64_t loc;
        uint32_t len;
        file.seekSet(8 + (i + myApp->currentLine) * 12);
        file.read(&loc, 8);
        file.read(&len, 4);

        // handle flags (high bit of len indicates last line)
        myApp->bookAtEnd = len >> 15;
        len &= 0x7FFF;
        file.seekSet(loc);
        char *line = (char *)malloc(len + 2);
        file.read(line, len);
        line[len] = '\n';
        line[len + 1] = 0;
        pageText.append(line);
        free(line);

        if (myApp->bookAtEnd) {
            break;
        }
    }

    uint64_t pos = file.curPosition();
    uint64_t len = file.fileSize();
    uint64_t textStart;
    file.seekSet(32);
    file.read(&textStart, 8);
    myApp->progressView->setProgress((float)(pos - textStart) / (float)(len - textStart));
    myApp->bookText->setText(pageText);
}
