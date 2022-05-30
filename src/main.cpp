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

    // View *parent = new View(32, 32, 236, 336);
    // window->addSubview(parent);

    // Button *child1 = new Button(32, 32, 128, 64, "Button 1");
    // Button *child2 = new Button(32, 128, 128, 64, "Button 2");
    // Button *child3 = new Button(32, 224, 128, 64, "Button 3");
    // parent->addSubview(child1);
    // parent->addSubview(child2);
    // parent->addSubview(child3);

    // child1->becomeFocused();

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
