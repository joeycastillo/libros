#include "Focus.h"
#include "OpenBookDevice.h"
#include "OpenBookWidgets.h"
#include <memory>

class OpenBookApplication : public Application {
public:
    OpenBookApplication(const std::shared_ptr<Window>& window);    
    bool locked = false;
    size_t currentLine;
    bool bookAtEnd;
    bool bookNeedsRefresh;

    std::string currentBook;

    std::vector<std::string> filenames;
    std::shared_ptr<View> mainMenu;
    std::shared_ptr<OpenBookTable> table;
    std::shared_ptr<OpenBookLabel> page;
};
