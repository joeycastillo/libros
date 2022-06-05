#include "Focus.h"
#include "OpenBook.h"

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
