#ifndef FatalErrorViewController_h
#define FatalErrorViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"
#include "OpenBookDatabase.h"

class FatalErrorViewController : public ViewController {
public:
    FatalErrorViewController(std::shared_ptr<Application> application, std::string message);
    void dismiss(Event event);
protected:
    virtual void createView() override;
    std::string message;
};

#endif // FatalErrorViewController_h
