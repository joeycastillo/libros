#ifndef BabelSetupViewController_h
#define BabelSetupViewController_h

#include "Focus.h"
#include "FocusWidgets.h"
#include "OpenBookWidgets.h"

class BabelSetupViewController : public ViewController {
public:
    BabelSetupViewController() {};
    void dismiss(Event event);
protected:
    virtual void createView() override;
    void updateProgress(Event event);
    std::shared_ptr<ProgressView> progressView;
};

#endif // BabelSetupViewController_h
