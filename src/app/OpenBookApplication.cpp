#include "OpenBookApplication.h"
#include "Tasks.h"

// Callbacks
void selectBook(std::shared_ptr<Application>application, Event event);
void turnPage(std::shared_ptr<Application>application, Event event);
void returnHome(std::shared_ptr<Application>application, Event event);
void lockScreen(std::shared_ptr<Application>application, Event event);

// Helpers
void updateBooks(OpenBookApplication *myApp);
void updatePage(std::shared_ptr<OpenBookApplication>myApp);

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window) : Application(window) {
    std::shared_ptr<Task> lockScreenTask = std::make_shared<OpenBookLockScreen>();
    this->addTask(lockScreenTask);
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>();
    this->addTask(inputTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>();
    this->addTask(displayTask);

    this->table = std::make_shared<BabelTable>(0, 0, 300, 400, 48, CellSelectionStyleInvert);
    this->page = std::make_shared<BabelLabel>(16, 16, 300 - 32, 400 - 32, "");

    window->addSubview(this->table);
    updateBooks(this);
    this->table->becomeFocused();

    // Actions for the home menu
    this->table->setAction(&selectBook, BUTTON_CENTER);

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
    OpenBook *book = OpenBook::sharedInstance();

    int32_t selectedIndex = event.userInfo;
    if (selectedIndex >= 0 && selectedIndex < (int32_t)myApp->filenames.size()) {
        myApp->currentBook = myApp->filenames[selectedIndex];
        std::string currentProgressFile = myApp->filenames[selectedIndex];
        currentProgressFile[currentProgressFile.size() - 1] = 'p';
        if (book->getSD()->exists(currentProgressFile.c_str())) {
            File f = book->getSD()->open(currentProgressFile.c_str(), FILE_READ);
            f.read(&myApp->currentLine, sizeof(size_t));
            f.close();
        } else {
            myApp->currentLine = 2; // start of lines, after title and author
            File f = book->getSD()->open(currentProgressFile.c_str(), FILE_WRITE);
            f.write((byte *)&(myApp->currentLine), sizeof(size_t));
            f.close();
        }

        window->removeSubview(myApp->table);
        window->addSubview(myApp->page);
        myApp->page->becomeFocused();
        updatePage(myApp);
    }
}

void turnPage(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();
    OpenBook *book = OpenBook::sharedInstance();

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
    File f = book->getSD()->open(currentProgressFile.c_str(), FILE_READ);
    f.write((byte *)&(myApp->currentLine), sizeof(size_t));
    f.close();

    updatePage(myApp);
}

void returnHome(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();

    window->removeSubview(myApp->page);
    window->addSubview(myApp->table);
    myApp->table->becomeFocused();
    window->setNeedsDisplay(true);
}

void lockScreen(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    myApp->locked = true;
}

void updateBooks(OpenBookApplication *myApp) {
    std::vector<std::string> titles;
    OpenBook *book = OpenBook::sharedInstance();

    File root = book->getSD()->open("/");
    File entry = root.openNextFile();
    while (entry) {
        if (!entry.isDirectory()) {
            uint64_t magic = 0;
            entry.read((void *)&magic, 8);
            if (magic == 5426643222204338255) { // the string "OPENBOOK"
                char *filename = (char *)malloc(128);
                entry.getName(filename, 128);
                File file = book->getSD()->open(filename);
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
    OpenBook *book = OpenBook::sharedInstance();
    std::string pageText = "";
    FatFile file = book->getSD()->open(myApp->currentBook.c_str(), FILE_READ);

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

    myApp->page->setText(pageText);
}
