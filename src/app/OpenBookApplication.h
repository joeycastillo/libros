#ifndef OpenBookApplication_h
#define OpenBookApplication_h

#include "Focus.h"
#include "OpenBookDatabase.h"
#include "OpenBookWidgets.h"
#include <memory>

class OpenBookApplication : public Application {
public:
    OpenBookApplication(const std::shared_ptr<Window>& window);    
    bool locked = false;

// Callbacks
    void selectBook(Event event);
    void turnPage(Event event);
    void returnHome(Event event);
    void lockScreen(Event event);
    void dismiss(Event event);
    void paginate(Event event);

    int requestedRefreshMode = -1;
};

#endif // OpenBookApplication_h
