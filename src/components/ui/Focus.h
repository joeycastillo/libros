#ifndef Focus_h
#define Focus_h

#include <stdint.h>
#include <vector>
#include <memory>
#include <map>
#include <functional>
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
    EventType type;
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
    virtual int16_t run(Application *application) = 0;
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
    void setAction(const Action &action, EventType type);
    void removeAction(EventType type);
    virtual std::weak_ptr<View>getSuperview();
    virtual std::weak_ptr<Window> getWindow();
    virtual void setWindow(std::shared_ptr<Window> window);
    Rect getFrame();
    void setFrame(Rect rect);
    bool isOpaque();
    void setOpaque(bool value);
    uint16_t getBackgroundColor();
    void setBackgroundColor(uint16_t value);
protected:
    bool focused = false;
    bool opaque = false;
    uint16_t backgroundColor = 0;
    uint16_t foregroundColor = 1;
    Rect frame = {0};
    DirectionalAffinity affinity = DirectionalAffinityVertical;
    std::vector<std::shared_ptr<View>> subviews;
    std::map<EventType, Action> actions;
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
    void setNeedsDisplayInRect(Rect rect, std::shared_ptr<View> view);
    Rect getDirtyRect();
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
};

class Application : public std::enable_shared_from_this<Application> {
public:
    Application(const std::shared_ptr<Window>& window);
    void run();
    void addTask(std::shared_ptr<Task> task);
    void generateEvent(EventType eventType, int32_t userInfo);
    std::shared_ptr<Window> getWindow();

    void setRootViewController(std::shared_ptr<ViewController> viewController);

protected:
    std::vector<std::shared_ptr<Task>> tasks;
    std::shared_ptr<Window> window;
    std::shared_ptr<ViewController> rootViewController;
};

class ViewController : public std::enable_shared_from_this<ViewController> {
public:
    ViewController() {};

    virtual void viewDidLoad() {};
    virtual void viewWillAppear();
    virtual void viewDidAppear() {};
    virtual void viewWillDisappear() {};
    virtual void viewDidDisappear();

    std::shared_ptr<View> getView();

protected:
    virtual void createView();
    void destroyView();
    std::shared_ptr<View> view;

    friend class Application;
};

#endif // Focus_h
