#include "Focus.h"
#include "OpenBook.h"

class OpenBookRawButtonInput : public Task {
public:
    OpenBookRawButtonInput(OpenBook *book);
    int16_t run(Application *application);
protected:
    OpenBook *book;
    uint8_t lastButtons = 0;
};

class OpenBookDisplay : public Task {
public:
    OpenBookDisplay(OpenBook *book);
    int16_t run(Application *application);
protected:
    OpenBook *book;
};
