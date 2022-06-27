#ifndef OpenBookTasks_h
#define OpenBookTasks_h

#include "Focus.h"
#include "OpenBookDevice.h"

class OpenBookRawButtonInput : public Task {
public:
    OpenBookRawButtonInput() {};
    bool run(std::shared_ptr<Application> application);
protected:
    uint8_t lastButtons = 0;
};

class OpenBookDisplay : public Task {
public:
    OpenBookDisplay() {};
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
    int32_t page = 0;
};

#endif // OpenBookTasks_h
