#ifndef Focus_h
#define Focus_h

#include <stdint.h>
#include <vector>
#include <map>
#include "Adafruit_GFX.h"

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

typedef struct {
    int16_t x;
    int16_t y;
} Point;

typedef struct {
    int16_t width;
    int16_t height;
} Size;

typedef struct {
    Point origin;
    Size size;
} Rect;

inline Rect MakeRect(int16_t x, int16_t y, int16_t width, int16_t height) { return {{x, y}, {width, height}}; }
inline bool RectsEqual(Rect a, Rect b) { return (a.origin.x == b.origin.x) && (a.origin.y == b.origin.y) && (a.size.width == b.size.width) && (a.size.height == b.size.height); }

class Application;
class Window;
class View;
class Task;

typedef struct {
    EventType type;
    View *originator;
    int32_t userInfo;
} Event;

typedef struct {
    View *up;
    View *down;
    View *left;
    View *right;
} FocusTarget;

typedef void (*Action)(Event);

class Task {
public:
    Task();
    virtual int16_t run(Application *application) = 0;
};

class View {
public:
    View(int16_t x, int16_t y, int16_t width, int16_t height);
    void draw(Adafruit_GFX *display, int16_t x, int16_t y);
    void addSubview(View *view);
    void removeSubview(View *view);
    void becomeFocused();
    void resignFocus();
    void movedToWindow();
    void willBecomeFocused();
    void didBecomeFocused();
    void willResignFocus();
    void didResignFocus();
    virtual bool handleEvent(Event event);
    void setAction(Action action, EventType type);
    void removeAction(EventType type);
    View *getSuperview();
    Rect getFrame();
    void setFrame(Rect rect);
protected:
    Rect frame;
    std::vector<View *> subviews;
    std::map<EventType, Action> actions;
    Window *window;
    View *superview;

    friend class Window;
};

class Window : public View {
public:
    Window(int16_t width, int16_t height);
    bool handleEvent(Event event) override;
    void setFocusTargets(View *view, View *up, View *right, View *down, View *left);
    bool needsDisplay();
    void setNeedsDisplay(bool needsDisplay);
    void setNeedsDisplayInRect(Rect rect, View *view);
    Rect getDirtyRect();
protected:
    Application *application;
    View *focusedView;
    bool dirty;
    Rect dirtyRect;
    std::map<View *, FocusTarget> focusTargets;

    friend class Application;
    friend class View;
};

class Application {
public:
    Application(Window *window);
    void run();
    void addTask(Task *task);
    void generateEvent(EventType eventType, int32_t userInfo);
    Window *getWindow();

protected:
    std::vector<Task *> tasks;
    Window *window;
};

#endif // Focus_h
