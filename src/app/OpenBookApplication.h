#include "Focus.h"
#include "OpenBook.h"

class OpenBookApplication : public Application {
public:
    OpenBookApplication(Window *window, OpenBook *book);
protected:
    OpenBook *book;
};
