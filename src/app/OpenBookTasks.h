#ifndef OpenBookTasks_h
#define OpenBookTasks_h

#include "Focus.h"
#include "OpenBookDevice.h"
#include "Adafruit_SPIFlash.h"

class OpenBookRawButtonInput : public Task {
public:
    OpenBookRawButtonInput() {};
    bool run(std::shared_ptr<Application> application);
protected:
    uint8_t lastButtons = 0;
};

class OpenBookDisplay : public Task {
public:
    OpenBookDisplay();
    bool run(std::shared_ptr<Application> application);
};

class OpenBookLockScreen : public Task {
public:
    OpenBookLockScreen() {};
    bool run(std::shared_ptr<Application> application);
};

class OpenBookPowerMonitor : public Task {
public:
    OpenBookPowerMonitor() {};
    bool run(std::shared_ptr<Application> application);
protected:
    int8_t wasOnBattery = -1;
};

class BurnBabelImage : public Task {
public:
    BurnBabelImage() {};
    bool run(std::shared_ptr<Application> application);
protected:
    File babelFile;
    int32_t page = -1;
    int32_t numPages = 0;
    int32_t lastUpdate = -1;
};

#endif // OpenBookTasks_h
