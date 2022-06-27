#ifndef OpenBookApplication_h
#define OpenBookApplication_h

#include "Focus.h"
#include "OpenBookDatabase.h"
#include "OpenBookWidgets.h"
#include "BookListViewController.h"
#include <memory>

class OpenBookApplication : public Application {
public:
    OpenBookApplication(const std::shared_ptr<Window>& window) : Application(window) {};

    void setup() override;

    bool locked = false;

    std::shared_ptr<BookListViewController> mainMenu;

// Callbacks
    void showLockScreen(Event event);
    void showBookReader(Event event);
    void returnHome(Event event);
    void changeRefreshMode(Event event);

    int requestedRefreshMode = -1;
};

#endif // OpenBookApplication_h
