#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"
#include "FocusWidgets.h"

OpenBook *book;
OpenBookApplication *application;

void setup() {
    book = new OpenBook();
    Window *window = new Window(300, 400);
    application = new OpenBookApplication(window, book);

    Table *table = new Table(0, 0, 300, 400, 48, CellSelectionStyleInvert);
    window->addSubview(table);

    std::vector<std::string> titles;
    titles.push_back("Pride and Prejudice");
    titles.push_back("Alice's Adventures in Wonderland");
    titles.push_back("The Adventures of Sherlock Holmes");
    titles.push_back("A Tale of Two Cities");
    titles.push_back("The Picture of Dorian Gray");
    titles.push_back("The Great Gatsby");

    table->setItems(titles);
    table->becomeFocused();

    application->run();
}

void loop() {
}
