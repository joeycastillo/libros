#ifndef BookListViewController_h
#define BookListViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"
#include "OpenBookDatabase.h"

class BookListViewController : public ViewController {
public:
    virtual void viewDidLoad() override;
    virtual void viewWillAppear() override;
    virtual void viewDidAppear() override;
    virtual void viewWillDisappear() override;
    virtual void viewDidDisappear() override;

    void selectBook(Event event);
    void dismiss(Event event);
    void paginate(Event event);
protected:
    virtual void createView() override;

    BookRecord currentBook;

    std::vector<BookRecord> books;
    std::shared_ptr<OpenBookTable> table;
    std::shared_ptr<BorderedView> modal;
};

#endif // BookListViewController_h
