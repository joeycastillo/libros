#include "FatalErrorViewController.h"
#include "OpenBookEvents.h"
#include "bitmaps.h"

FatalErrorViewController::FatalErrorViewController(std::shared_ptr<Application> application, std::string message) : ViewController(application) {
    this->message = message;
}

void FatalErrorViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<View>(MakeRect(0, 0, 300, 400));
    this->view->setOpaque(true);
    this->view->setBackgroundColor(EPD_DARK);
    std::shared_ptr<BorderedView> modal = std::make_shared<BorderedView>(MakeRect(40, 150, 220, 100));
    modal->setOpaque(true);
    std::shared_ptr<Label> label = std::make_shared<Label>(MakeRect(20, 20, 180, 8), this->message);
    modal->addSubview(label);
    std::shared_ptr<Button> button = std::make_shared<Button>(MakeRect(20, 60, 180, 20), "Restart Now");
    modal->addSubview(button);
    this->view->addSubview(modal);
    this->view->setAction(std::bind(&FatalErrorViewController::dismiss, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_TAP);
}

void FatalErrorViewController::dismiss(Event event) {
    OpenBookDevice::sharedDevice()->reset();
}
