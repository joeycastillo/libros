#include "Focus.h"
#include "OpenBook.h"

class OpenBookApplication : public Application {
public:
    OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book);
protected:
    OpenBook *book;
};
