#include "Focus.h"
#include "OpenBook.h"
#include "BabelWidgets.h"
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
    std::shared_ptr<BabelTable> table;
    std::shared_ptr<BabelLabel> page;
};
