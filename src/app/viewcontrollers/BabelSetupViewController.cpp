#include "BabelSetupViewController.h"
#include "OpenBookEvents.h"
#include "bitmaps.h"
#include "OpenBookTasks.h"
#include "OpenBookDevice.h"

void BabelSetupViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<HatchedView>(MakeRect(0, 0, 300, 400), EPD_BLACK);
    std::shared_ptr<BorderedView> modal = std::make_shared<BorderedView>(MakeRect(40, 150, 220, 100));
    modal->setOpaque(true);
    if (!OpenBookDevice::sharedDevice()->fileExists("babel.bin")) {
        std::shared_ptr<Label> label1 = std::make_shared<Label>(MakeRect(20, 20, 180, 8), "Language chip not initialized.");
        modal->addSubview(label1);
        std::shared_ptr<Label> label2 = std::make_shared<Label>(MakeRect(20, 30, 180, 8), "Copy babel.bin to your SD card");
        modal->addSubview(label2);
        std::shared_ptr<Label> label3 = std::make_shared<Label>(MakeRect(20, 40, 180, 8), "and restart the device.");
        modal->addSubview(label3);
        std::shared_ptr<Button> button = std::make_shared<Button>(MakeRect(20, 60, 180, 20), "Restart Now");
        modal->addSubview(button);
        this->view->setAction(std::bind(&BabelSetupViewController::dismiss, this, std::placeholders::_1), FOCUS_EVENT_BUTTON_TAP);
    } else {
        std::shared_ptr<Label> label1 = std::make_shared<Label>(MakeRect(20, 20, 180, 8), "Flashing language chip...");
        modal->addSubview(label1);
        std::shared_ptr<ProgressView> progressView = std::make_shared<ProgressView>(MakeRect(20, 60, 180, 20));
        modal->addSubview(progressView);
        this->view->setAction(std::bind(&BabelSetupViewController::updateProgress, this, std::placeholders::_1), OPEN_BOOK_EVENT_PROGRESS);
        // TODO: Flash language chip.
    }
    this->view->addSubview(modal);
}

void BabelSetupViewController::dismiss(Event event) {
    OpenBookDevice::sharedDevice()->reset();
}

void BabelSetupViewController::updateProgress(Event event) {
    this->progressView->setProgress((float) event.userInfo / 100.0);
}
