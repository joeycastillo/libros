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
    void saveProgress(Event event);
    void showMenu(Event event);
    
    void handleModal(Event event);

protected:
    virtual void createView() override;
    void _updateView();

    BookRecord book = {};
    int32_t currentPage = 0;
    int32_t numPages = 1;

    std::shared_ptr<Control> eventReceiver;
    std::shared_ptr<ProgressView> progressView;
    std::shared_ptr<Label> progressLabel;
    std::shared_ptr<OpenBookLabel> bookText;

    std::shared_ptr<Control> modal;
    std::shared_ptr<ProgressView> modalSlider;
    std::shared_ptr<OpenBookLabel> gotoPageLabel;
};

#endif // BookReaderViewController_h
