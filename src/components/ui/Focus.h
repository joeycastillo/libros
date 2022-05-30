#ifndef Focus_h
#define Focus_h

#include <stdint.h>
#include <vector>
#include <memory>
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

typedef enum {
    DirectionalAffinityVertical,
    DirectionalAffinityHorizontal,
} DirectionalAffinity;

typedef void (*Action)(Event);

class Task {
public:
    Task();
    virtual int16_t run(Application *application) = 0;
};

class View : public std::enable_shared_from_this<View> {
public:
    View(int16_t x, int16_t y, int16_t width, int16_t height);
    virtual void draw(Adafruit_GFX *display, int16_t x, int16_t y);
    virtual void addSubview(View *view);
    void removeSubview(View *view);
    virtual void becomeFocused();
    virtual void resignFocus();
    virtual void movedToWindow();
    virtual void willBecomeFocused();
    virtual void didBecomeFocused();
    virtual void willResignFocus();
    virtual void didResignFocus();
    virtual bool handleEvent(Event event);
    void setAction(Action action, EventType type);
    void removeAction(EventType type);
    View *getSuperview();
    Rect getFrame();
    void setFrame(Rect rect);
protected:
    Rect frame;
    DirectionalAffinity affinity = DirectionalAffinityVertical;
    std::vector<View *> subviews;
    std::map<EventType, Action> actions;
    std::weak_ptr<Window> window;
    View *superview;

    friend class Window;
};

class Window : public View {
public:
    Window(int16_t width, int16_t height);
    void addSubview(View *view) override;
    bool needsDisplay();
    void setNeedsDisplay(bool needsDisplay);
    void setNeedsDisplayInRect(Rect rect, View *view);
    Rect getDirtyRect();
    View* getFocusedView();
protected:
    std::weak_ptr<Application> application;
    View *focusedView;
    bool dirty;
    Rect dirtyRect;

    friend class Application;
    friend class View;
};

class Application : public std::enable_shared_from_this<Application> {
public:
    Application(const std::shared_ptr<Window>& window);
    void run();
    void addTask(Task *task);
    void generateEvent(EventType eventType, int32_t userInfo);
    std::shared_ptr<Window> getWindow();

protected:
    std::vector<Task *> tasks;
    std::shared_ptr<Window> window;
};

#endif // Focus_h
