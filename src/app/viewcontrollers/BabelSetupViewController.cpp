#include "BabelSetupViewController.h"
#include "OpenBookEvents.h"
#include "bitmaps.h"
#include "OpenBookTasks.h"
#include "OpenBookDevice.h"

void BabelSetupViewController::createView() {
    ViewController::createView();
    this->view = std::make_shared<Control>(MakeRect(0, 0, 300, 400));
    this->view->setOpaque(true);
    this->view->setBackgroundColor(EPD_DARK);
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
        this->view->addSubview(modal);
    } else {
        std::shared_ptr<Label> label1 = std::make_shared<Label>(MakeRect(20, 20, 180, 8), "Flashing language chip.");
        modal->addSubview(label1);
        std::shared_ptr<Label> label2 = std::make_shared<Label>(MakeRect(20, 40, 180, 8), "This will take about 1 minute.");
        modal->addSubview(label2);
        // FIXME: all the setNeedsDisplayInRect calls are broken and
        // only work when the view is in screen coordinates.
        this->progressView = std::make_shared<ProgressView>(MakeRect(60, 210, 180, 20));
        this->progressView->setForegroundColor(EPD_BLACK);
        this->progressView->setBackgroundColor(EPD_WHITE);
        this->progressView->setOpaque(true);
        this->view->setAction(std::bind(&BabelSetupViewController::updateProgress, this, std::placeholders::_1), OPEN_BOOK_EVENT_PROGRESS);
        std::shared_ptr<BurnBabelImage> flashLanguageChip = std::make_shared<BurnBabelImage>();
        if (std::shared_ptr<Application> app = this->application.lock()) {
            app->addTask(flashLanguageChip);
        }
        this->view->addSubview(modal);
        this->view->addSubview(this->progressView);
    }
}

void BabelSetupViewController::dismiss(Event event) {
    OpenBookDevice::sharedDevice()->reset();
}

void BabelSetupViewController::updateProgress(Event event) {
    if (event.userInfo % 4 == 0) {
        float progress = (float) (event.userInfo + 4.0) / 100.0;
        this->generateEvent(OPEN_BOOK_EVENT_REQUEST_REFRESH_MODE, OPEN_BOOK_DISPLAY_MODE_FASTPARTIAL);
        this->progressView->setProgress(progress);
    }
}
