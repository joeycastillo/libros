#include "OpenBookApplication.h"
#include "Tasks.h"

void selectBook(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApp = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApp->getWindow();

    // TODO: Remove table from view hierarchy and add reader view
}

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book) : Application(window) {
    this->book = book;
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>(book);
    this->addTask(inputTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>(book);
    this->addTask(displayTask);

    this->table = std::make_shared<Table>(0, 0, 300, 400, 48, CellSelectionStyleInvert);
    window->addSubview(this->table);
    std::vector<std::string> titles;

    File root = this->book->getSD()->open("/");
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
                free(title);
            }
        }
        entry = root.openNextFile();
    }

    this->table->setItems(titles);
    this->table->becomeFocused();

    this->table->setAction(&selectBook, BUTTON_CENTER);
}
