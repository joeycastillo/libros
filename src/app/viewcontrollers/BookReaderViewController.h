#ifndef BookReaderViewController_h
#define BookReaderViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"
#include "OpenBookDatabase.h"

class BookReaderViewController : public ViewController {
public:
    virtual void viewDidLoad() override;
    virtual void viewWillAppear() override;
    virtual void viewDidAppear() override;
    virtual void viewWillDisappear() override;
    virtual void viewDidDisappear() override;

    void turnPage(Event event);
    void returnHome(Event event);

protected:
    virtual void createView() override;

    BookRecord currentBook;
    uint32_t currentPage = 0;

    std::shared_ptr<Control> page;
    std::shared_ptr<ProgressView> progressView;
    std::shared_ptr<OpenBookLabel> bookText;
};

#endif // BookReaderViewController_h
