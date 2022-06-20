#ifndef OpenBookTasks_h
#define OpenBookTasks_h

#include "Focus.h"
#include "OpenBookDevice.h"

class OpenBookRawButtonInput : public Task {
public:
    OpenBookRawButtonInput() {};
    int16_t run(Application *application);
protected:
    uint8_t lastButtons = 0;
};

class OpenBookDisplay : public Task {
public:
    OpenBookDisplay() {};
    int16_t run(Application *application);
};

class OpenBookLockScreen : public Task {
public:
    OpenBookLockScreen() {};
    int16_t run(Application *application);
};

class OpenBookPowerMonitor : public Task {
public:
    OpenBookPowerMonitor() {};
    int16_t run(Application *application);
protected:
    int8_t wasOnBattery = -1;
};

#endif // OpenBookTasks_h
