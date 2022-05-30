#include "Focus.h"
#include "Adafruit_EPD.h"
#include <algorithm>

Task::Task() {
}

View::View(int16_t x, int16_t y, int16_t width, int16_t height) {
    this->frame = MakeRect(x, y, width, height);
    this->window = NULL;
    this->superview = NULL;
}

void View::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    if (this->window->focusedView == this) {
        display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, EPD_BLACK);
    } else {
        display->drawRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, EPD_BLACK);
    }
    for(View *view : this->subviews) {
        view->draw(display, this->frame.origin.x, this->frame.origin.y);
    }
}

void View::addSubview(View *view) {
    view->superview = this;
    view->window = this->window;
    this->subviews.push_back(view);
    this->window->setNeedsDisplay(true);
}

void View::removeSubview(View *view) {
    view->superview = NULL;
    view->window = NULL;
    int index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), view));
    this->subviews.erase(this->subviews.begin() + index);
    this->window->setNeedsDisplay(true);
}

void View::becomeFocused() {
    View *oldResponder = this->window->focusedView;
    oldResponder->willResignFocus();
    this->window->focusedView = NULL;
    oldResponder->didResignFocus();
    this->willBecomeFocused();
    this->window->focusedView = this;
    this->didBecomeFocused();
}

void View::resignFocus() {
    if (this->window->focusedView != this) return;
    this->window->becomeFocused();
}

void View::movedToWindow() {
    // nothing to do here
}

void View::willBecomeFocused() {
    // nothing to do here
}

void View::didBecomeFocused() {
    if (this->superview != NULL) this->window->setNeedsDisplayInRect(this->frame, this);
}

void View::willResignFocus() {
    // nothing to do here
}

void View::didResignFocus() {
    if (this->superview != NULL) this->window->setNeedsDisplayInRect(this->frame, this);
}

bool View::handleEvent(Event event) {
    if (this->actions.count(event.type)) {
        this->actions[event.type](event);
    } else if (this->superview == this->window) {
        this->window->handleEvent(event);
    } else {
        this->superview->handleEvent(event);
    }

    return false;
}

void View::setAction(Action action, EventType type) {
    this->actions[type] = action;
}

void View::removeAction(EventType type) {
    // TODO: remove the action
}

View* View::getSuperview() {
    return this->superview;
}

Rect View::getFrame() {
    return this->frame;
}

void View::setFrame(Rect frame) {
    Rect dirtyRect = MakeRect(min(this->frame.origin.x, frame.origin.x), min(this->frame.origin.y, frame.origin.y), 0, 0);
    dirtyRect.size.width = max(this->frame.origin.x + this->frame.size.width, frame.origin.x + frame.size.width) - dirtyRect.origin.x;
    dirtyRect.size.height = max(this->frame.origin.y + this->frame.size.height, frame.origin.y + frame.size.height) - dirtyRect.origin.y;
    this->frame = frame;
    this->window->setNeedsDisplayInRect(dirtyRect, this);
}

Application::Application(Window *window) {
    this->window = window;
    this->window->application = this;
}

void Application::addTask(Task *task) {
    this->tasks.push_back(task);
}

void Application::run() {
    while(true) {
        for(Task *task : this->tasks) {
            if (task->run(this) != 0) return;
        }
    }
}

void Application::generateEvent(EventType eventType, int32_t userInfo) {
    Event event;
    event.type = eventType;
    event.userInfo = userInfo;
    event.originator = this->window->focusedView;
    this->window->focusedView->handleEvent(event);
}

Window* Application::getWindow() {
    return this->window;
}

Window::Window(int16_t width, int16_t height) : View(0, 0, width, height) {
    this->focusedView = this;
    this->window = this;
    this->dirtyRect = MakeRect(0, 0, width, height);
}

bool Window::handleEvent(Event event) {
    if (event.type > BUTTON_RIGHT) {
        return View::handleEvent(event);
    }

    View *focusSource = this->focusedView;
    while (focusSource != this && !(this->focusTargets.count(focusSource))) {
        focusSource = focusSource->superview;
    }

    View *focusTarget = NULL;

    switch (event.type) {
        case BUTTON_UP:
            focusTarget = this->focusTargets[focusSource].up;
            break;
        case BUTTON_DOWN:
            focusTarget = this->focusTargets[focusSource].down;
            break;
        case BUTTON_LEFT:
            focusTarget = this->focusTargets[focusSource].left;
            break;
        case BUTTON_RIGHT:
            focusTarget = this->focusTargets[focusSource].right;
            break;
        default:
            break;
    }

    if (focusTarget == NULL) {
        return View::handleEvent(event);
    } else {
        focusTarget->becomeFocused();
        return true;
    }
}

void Window::setFocusTargets(View *view, View *up, View *right, View *down, View *left) {
    FocusTarget target;
    target.up = up;
    target.down = down;
    target.left = left;
    target.right = right;
    this->focusTargets[view] = target;
}

bool Window::needsDisplay() {
    return (this->dirtyRect.size.width > 0 && this->dirtyRect.size.height > 0);
}

void Window::setNeedsDisplay(bool needsDisplay) {
    if (needsDisplay) {
        this->dirtyRect = MakeRect(0, 0, this->frame.size.width, this->frame.size.height);
    } else {
        this->dirtyRect = MakeRect(0, 0, 0, 0);
    }
}

void Window::setNeedsDisplayInRect(Rect rect, View *view) {
    View *superview = view; // will become superview shortly
    do {
        superview = superview->superview;
        rect.origin.x += superview->frame.origin.x;
        rect.origin.y += superview->frame.origin.y;
    } while (superview != this);

    Rect dirtyRect = MakeRect(min(this->dirtyRect.origin.x, rect.origin.x), min(this->dirtyRect.origin.y, rect.origin.y), 0, 0);
    dirtyRect.size.width = max(this->dirtyRect.origin.x + this->dirtyRect.size.width, rect.origin.x + rect.size.width) - dirtyRect.origin.x;
    dirtyRect.size.height = max(this->dirtyRect.origin.y + this->dirtyRect.size.height, rect.origin.y + rect.size.height) - dirtyRect.origin.y;

    this->dirtyRect = dirtyRect;
}

Rect Window::getDirtyRect() {
    return this->dirtyRect;
}
