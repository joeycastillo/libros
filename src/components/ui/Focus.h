#ifndef Focus_h
#define Focus_h

#include <stdint.h>
#include <Arduino.h>
#include <vector>
#include <map>

typedef enum {
    BUTTON_LEFT,
    BUTTON_DOWN,
    BUTTON_UP,
    BUTTON_RIGHT,
    BUTTON_CENTER,
    BUTTON_PREV,
    BUTTON_NEXT,
    BUTTON_LOCK,
} EventType;

class Application;
class Window;
class View;
class Task;

typedef struct {
    EventType type;
    View *originator;
    int32_t userInfo;
} Event;

typedef void (*Action)(Event);

class Task {
public:
    Task();
    virtual int16_t run(Application *application) = 0;
};

class View {
public:
    View(int16_t x, int16_t y, int16_t width, int16_t height);
    void draw(int16_t x, int16_t y);
    void addSubview(View *view);
    void removeSubview(View *view);
    void becomeFocused();
    void resignFocus();
    void movedToWindow();
    void willBecomeFocused();
    void didBecomeFocused();
    void willResignFocus();
    void didResignFocus();
    void handleEvent(Event event);
    void setAction(Action action, EventType type);
    void removeAction(EventType type);
protected:
    std::vector<View *> subviews;
    std::map<EventType, Action> actions;
    int16_t x, y, width, height;
    Window *window;
    View *nextResponder;
};

class Window : public View {
public:
    Window(int16_t x, int16_t y, int16_t width, int16_t height);
    void setFocusTargets(View *view, View *up, View *right, View *down, View *left);
    bool needsDisplay();
    void setNeedsDisplay();
protected:
    Application *application;
    View *focusedView;

    friend class Application;
};

class Application {
public:
    Application(Window *window);
    void run();
    void addTask(Task *task);
    void generateEvent(EventType eventType, int32_t userInfo);

protected:
    std::vector<Task *> tasks;
    Window *window;
};

#endif // Focus_h
