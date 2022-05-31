#include "Focus.h"
#include "OpenBook.h"
#include "FocusWidgets.h"
#include <memory>

class OpenBookApplication : public Application {
public:
    OpenBookApplication(const std::shared_ptr<Window>& window, OpenBook *book);    
    size_t currentLine;
    bool bookAtEnd;
    bool bookNeedsRefresh;

    std::string currentBook;

    std::vector<std::string> filenames;
    std::shared_ptr<Table> table;
    std::shared_ptr<Label> page;
    OpenBook *book;
};
