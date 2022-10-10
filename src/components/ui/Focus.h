#ifndef Focus_h
#define Focus_h

#include <stdint.h>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "Adafruit_GFX.h"

#define FOCUS_EVENT_BUTTON_LEFT (0)
#define FOCUS_EVENT_BUTTON_DOWN (1)
#define FOCUS_EVENT_BUTTON_UP (2)
#define FOCUS_EVENT_BUTTON_RIGHT (3)
#define FOCUS_EVENT_BUTTON_TAP (4)
#define FOCUS_EVENT_BUTTON_PREV (5)
#define FOCUS_EVENT_BUTTON_NEXT (6)
#define FOCUS_EVENT_BUTTON_LOCK (7)

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

inline Point MakePoint(int16_t x, int16_t y) { return {x, y}; }
inline Size MakeSize(int16_t width, int16_t height) { return {width, height}; }
inline Rect MakeRect(int16_t x, int16_t y, int16_t width, int16_t height) { return {{x, y}, {width, height}}; }

inline bool PointsEqual(Point a, Point b) { return (a.x == b.x) && (a.y == b.y); }
inline bool SizesEqual(Size a, Size b) { return (a.width == b.width) && (a.height == b.height); }
inline bool RectsEqual(Rect a, Rect b) { return PointsEqual(a.origin, b.origin) && SizesEqual(a.size, b.size); }

class Application;
class Window;
class View;
class Task;
class ViewController;

typedef struct {
    int32_t type;
    int32_t userInfo;
} Event;

typedef enum {
    DirectionalAffinityVertical,
    DirectionalAffinityHorizontal,
} DirectionalAffinity;

typedef std::function<void(Event)> Action;

class Task {
public:
    Task();
    virtual bool run(std::shared_ptr<Application> application) = 0;
};

class View : public std::enable_shared_from_this<View> {
public:
    View(Rect rect);
    ~View();
    virtual void draw(Adafruit_GFX *display, int16_t x, int16_t y);
    virtual void addSubview(std::shared_ptr<View> view);
    void removeSubview(std::shared_ptr<View> view);
    bool isFocused();
    virtual bool canBecomeFocused();
    virtual bool becomeFocused();
    virtual void resignFocus();
    virtual void movedToWindow();
    virtual void willBecomeFocused();
    virtual void didBecomeFocused();
    virtual void willResignFocus();
    virtual void didResignFocus();
    virtual bool handleEvent(Event event);
    void setAction(const Action &action, int32_t type);
    void removeAction(int32_t type);
    virtual std::weak_ptr<View>getSuperview();
    virtual std::weak_ptr<Window> getWindow();
    virtual void setWindow(std::shared_ptr<Window> window);
    Rect getFrame();
    void setFrame(Rect rect);
    bool isOpaque();
    void setOpaque(bool value);
    bool isHidden();
    void setHidden(bool value);
    int32_t getTag();
    void setTag(int32_t value);
    uint16_t getBackgroundColor();
    void setBackgroundColor(uint16_t value);
    uint16_t getForegroundColor();
    void setForegroundColor(uint16_t value);
    uint16_t getDirectionalAffinity();
    void setDirectionalAffinity(DirectionalAffinity value);
    void setNeedsDisplayInRect(Rect rect);
protected:
    bool focused = false;
    bool opaque = false;
    bool hidden = false;
    int32_t tag = 0;
    uint16_t backgroundColor = 0;
    uint16_t foregroundColor = 1;
    Rect frame = {0};
    DirectionalAffinity affinity = DirectionalAffinityVertical;
    std::vector<std::shared_ptr<View>> subviews;
    std::map<int32_t, Action> actions;
    std::weak_ptr<View> superview;
private:
    std::weak_ptr<Window> window;

    friend class Window;
};

class Control : public View {
public:
    Control(Rect rect);
    bool isEnabled();
    void setEnabled(bool value);
    bool canBecomeFocused() override;
protected:
    bool enabled = true;
};

class Window : public View {
public:
    Window(Size size);
    void addSubview(std::shared_ptr<View> view) override;
    bool canBecomeFocused() override;
    bool needsDisplay();
    void setNeedsDisplay(bool needsDisplay);
    Rect getDirtyRect();
    void setNeedsDisplayInRect(Rect rect);
    std::weak_ptr<View> getFocusedView();
    std::weak_ptr<View>getSuperview() override;
    std::weak_ptr<Window> getWindow() override;
    void setWindow(std::shared_ptr<Window> window) override;
protected:
    std::weak_ptr<Application> application;
    std::weak_ptr<View> focusedView;
    bool dirty;
    Rect dirtyRect;

    friend class Application;
    friend class View;
    friend class ViewController;
};

class Application : public std::enable_shared_from_this<Application> {
public:
    Application(const std::shared_ptr<Window>& window);

    virtual void setup() = 0;
    void run();

    void addTask(std::shared_ptr<Task> task);
    void generateEvent(int32_t eventType, int32_t userInfo);
    std::shared_ptr<Window> getWindow();

    void setRootViewController(std::shared_ptr<ViewController> viewController);

protected:
    std::vector<std::shared_ptr<Task>> tasks;
    std::shared_ptr<Window> window;
    std::shared_ptr<ViewController> rootViewController;
};

class ViewController : public std::enable_shared_from_this<ViewController> {
public:
    ViewController(std::shared_ptr<Application> application);

    virtual void viewWillAppear();
    virtual void viewDidAppear() {};
    virtual void viewWillDisappear() {};
    virtual void viewDidDisappear();

    void generateEvent(int32_t eventType, int32_t userInfo = 0);

protected:
    virtual void createView();
    virtual void destroyView();
    std::shared_ptr<View> view;
    std::weak_ptr<Application> application;

    friend class Application;
};

#endif // Focus_h
