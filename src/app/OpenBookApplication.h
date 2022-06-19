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
    size_t currentLine;
    bool bookAtEnd;
    bool bookNeedsRefresh;

    BookRecord currentBook;
    uint32_t currentPage = 0;

// Callbacks
    void selectBook(Event event);
    void turnPage(Event event);
    void returnHome(Event event);
    void lockScreen(Event event);
    void dismiss(Event event);
    void paginate(Event event);

    std::vector<BookRecord> books;
    std::shared_ptr<View> mainMenu;
    std::shared_ptr<OpenBookTable> table;
    std::shared_ptr<Control> page;
    std::shared_ptr<ProgressView> progressView;
    std::shared_ptr<OpenBookLabel> bookText;
    std::shared_ptr<BorderedView> modal;

    int requestedRefreshMode = -1;
};

#endif // OpenBookApplication_h
