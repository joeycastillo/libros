#ifndef BookListViewController_h
#define BookListViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"
#include "OpenBookDatabase.h"

class BookListViewController : public ViewController {
public:
    virtual void viewWillAppear() override;

    void selectBook(Event event);
    void dismiss(Event event);
    void paginate(Event event);
protected:
    virtual void createView() override;

    BookRecord currentBook = {0};

    std::vector<BookRecord> books;
    std::shared_ptr<OpenBookTable> table;
    std::shared_ptr<BorderedView> modal;
};

#endif // BookListViewController_h
