#ifndef BookReaderViewController_h
#define BookReaderViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"
#include "OpenBookDatabase.h"

class BookReaderViewController : public ViewController {
public:
    BookReaderViewController(std::shared_ptr<Application> application, BookRecord book);
    virtual void viewWillAppear() override;

    void turnPage(Event event);
    void returnHome(Event event);

protected:
    virtual void createView() override;
    void _updateView();

    BookRecord book = {0};
    uint32_t currentPage = 0;
    uint32_t numPages = 1;

    std::shared_ptr<Control> eventReceiver;
    std::shared_ptr<ProgressView> progressView;
    std::shared_ptr<Label> progressLabel;
    std::shared_ptr<OpenBookLabel> bookText;
};

#endif // BookReaderViewController_h
