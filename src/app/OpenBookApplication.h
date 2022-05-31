#include "Focus.h"
#include "OpenBook.h"
#include "FocusWidgets.h"
#include <memory>

class OpenBookApplication : public Application {
public:
    OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book);
    std::shared_ptr<Table> table;
protected:
    OpenBook *book;
};
