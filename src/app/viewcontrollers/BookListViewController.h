#ifndef BookListViewController_h
#define BookListViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"
#include "OpenBookDatabase.h"

class BookListViewController : public ViewController {
public:
    BookListViewController(std::shared_ptr<Application> application) : ViewController(application) {};
    virtual void viewWillAppear() override;

    void selectBook(Event event);
    void dismiss(Event event);
    void paginate(Event event);
    void updateBatteryIcon(Event event = {0});
protected:
    virtual void createView() override;

    std::vector<BookRecord> books;
    BookRecord currentBook = {0};

    std::shared_ptr<OpenBookTable> table;
    std::shared_ptr<BorderedView> modal;
    std::shared_ptr<BitmapView> batteryIcon;
    std::shared_ptr<BitmapView> usbIcon;
    std::shared_ptr<Label> voltageLabel;
};

#endif // BookListViewController_h
