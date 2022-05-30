#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"
#include "FocusWidgets.h"
#include <memory>

OpenBook *book;
std::shared_ptr<OpenBookApplication> application(nullptr);
std::shared_ptr<Window> window(nullptr);
std::shared_ptr<Table> table(nullptr);

void centerButtonPressed(Event event) {
    if (table) {
        window->becomeFocused();
        window->removeSubview(table);
        table.reset();
        window->setNeedsDisplay(true);
    } else {
        table = std::make_shared<Table>(0, 0, 300, 400, 48, CellSelectionStyleInvert);
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
    }
}

void setup() {
    book = new OpenBook();
    window = std::make_shared<Window>(300, 400);
    application = std::make_shared<OpenBookApplication>(window, book);
    window->becomeFocused();
    window->setNeedsDisplay(true);
    window->setAction(&centerButtonPressed, BUTTON_CENTER);
}

void loop() {
    application->run();
}
