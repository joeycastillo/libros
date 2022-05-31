#include "OpenBookApplication.h"
#include "Tasks.h"

void centerButtonPressed(std::shared_ptr<Application>application, Event event) {
    std::shared_ptr<OpenBookApplication>myApplication = std::static_pointer_cast<OpenBookApplication, Application>(application);
    std::shared_ptr<Window>window = myApplication->getWindow();

    if (myApplication->table) {
        window->becomeFocused();
        window->removeSubview(myApplication->table);
        myApplication->table.reset();
        window->setNeedsDisplay(true);
    } else {
        myApplication->table = std::make_shared<Table>(0, 0, 300, 400, 48, CellSelectionStyleInvert);
        window->addSubview(myApplication->table);

        std::vector<std::string> titles;
        titles.push_back("Pride and Prejudice");
        titles.push_back("Alice's Adventures in Wonderland");
        titles.push_back("The Adventures of Sherlock Holmes");
        titles.push_back("A Tale of Two Cities");
        titles.push_back("The Picture of Dorian Gray");
        titles.push_back("The Great Gatsby");

        myApplication->table->setItems(titles);
        myApplication->table->becomeFocused();
    }
}

OpenBookApplication::OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book) : Application(window) {
    this->book = book;
    std::shared_ptr<Task> inputTask = std::make_shared<OpenBookRawButtonInput>(book);
    this->addTask(inputTask);
    std::shared_ptr<Task> displayTask = std::make_shared<OpenBookDisplay>(book);
    this->addTask(displayTask);

    window->setAction(&centerButtonPressed, BUTTON_CENTER);
}
