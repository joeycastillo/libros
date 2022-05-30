#include "Focus.h"
#include <algorithm>

Task::Task() {
}

View::View(int16_t x, int16_t y, int16_t width, int16_t height) {
    this->frame = MakeRect(x, y, width, height);
    this->window.reset();
    this->superview = NULL;
}

void View::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    for(View *view : this->subviews) {
        view->draw(display, this->frame.origin.x, this->frame.origin.y);
    }
}

void View::addSubview(View *view) {
    view->superview = this;
    this->subviews.push_back(view);
    if (std::shared_ptr<Window> window = this->window.lock()) {
        view->window = window;
        window->setNeedsDisplay(true);
    }
}

void View::removeSubview(View *view) {
    view->superview = NULL;
    view->window.reset();
    int index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), view));
    this->subviews.erase(this->subviews.begin() + index);
    if (std::shared_ptr<Window> window = this->window.lock()) {
        window->setNeedsDisplay(true);
    }
}

void View::becomeFocused() {
    if (std::shared_ptr<Window> window = this->window.lock()) {
        View *oldResponder = window->focusedView;
        if (oldResponder != NULL) {
            oldResponder->willResignFocus();
            window->focusedView = NULL;
            oldResponder->didResignFocus();
        }
        willBecomeFocused();
        window->focusedView = this;
        this->didBecomeFocused();
    }
}

void View::resignFocus() {
    if (std::shared_ptr<Window> window = this->window.lock()) {
        if (window->focusedView != this) return;
        window->becomeFocused();
    }
}

void View::movedToWindow() {
    // nothing to do here
}

void View::willBecomeFocused() {
    // nothing to do here
}

void View::didBecomeFocused() {
    if (this->superview != NULL) {
        if (std::shared_ptr<Window> window = this->window.lock()) {
            window->setNeedsDisplayInRect(this->frame, this);
        }
    }
}

void View::willResignFocus() {
    // nothing to do here
}

void View::didResignFocus() {
    if (this->superview != NULL) {
        if (std::shared_ptr<Window> window = this->window.lock()) {
            window->setNeedsDisplayInRect(this->frame, this);
        }
    }
}

bool View::handleEvent(Event event) {
    View *focusedView = NULL;
    if (std::shared_ptr<Window> window = this->window.lock()) {
        focusedView = window->getFocusedView();
    }

    if (focusedView == NULL) return false;

    if (this->actions.count(event.type)) {
        this->actions[event.type](event);
    } else if (event.type < BUTTON_CENTER) {
        uint32_t index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
        switch (event.type) {
            case BUTTON_UP:
                if (this->affinity == DirectionalAffinityVertical && index > 0) {
                    this->subviews[index - 1]->becomeFocused();
                    return true;
                }
                break;
            case BUTTON_DOWN:
                if (this->affinity == DirectionalAffinityVertical && (index + 1) < this->subviews.size()) {
                    this->subviews[index + 1]->becomeFocused();
                    return true;
                }
                break;
            case BUTTON_LEFT:
                if (this->affinity == DirectionalAffinityHorizontal && index > 0) {
                    this->subviews[index - 1]->becomeFocused();
                    return true;
                }
                break;
            case BUTTON_RIGHT:
                if (this->affinity == DirectionalAffinityHorizontal && (index + 1) < this->subviews.size()) {
                    this->subviews[index + 1]->becomeFocused();
                    return true;
                }
                break;
            default:
                break;
        }
    }
    if (this->superview != NULL) {
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
    if (std::shared_ptr<Window> window = this->window.lock()) {
        Rect dirtyRect = MakeRect(min(this->frame.origin.x, frame.origin.x), min(this->frame.origin.y, frame.origin.y), 0, 0);
        dirtyRect.size.width = max(this->frame.origin.x + this->frame.size.width, frame.origin.x + frame.size.width) - dirtyRect.origin.x;
        dirtyRect.size.height = max(this->frame.origin.y + this->frame.size.height, frame.origin.y + frame.size.height) - dirtyRect.origin.y;
        this->frame = frame;
        window->setNeedsDisplayInRect(dirtyRect, this);
    }
}

Application::Application(const std::shared_ptr<Window>& window) {
    this->window = window;
}

void Application::addTask(Task *task) {
    this->tasks.push_back(task);
}

void Application::run() {
    this->window->application = this->shared_from_this();
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

std::shared_ptr<Window> Application::getWindow() {
    return this->window;
}

Window::Window(int16_t width, int16_t height) : View(0, 0, width, height) {
    this->focusedView = this;
    this->dirtyRect = MakeRect(0, 0, width, height);
}

void Window::addSubview(View *view) {
    view->window = std::static_pointer_cast<Window>(this->shared_from_this());
    View::addSubview(view);
}

bool Window::needsDisplay() {
    return this->dirty;
}

void Window::setNeedsDisplay(bool needsDisplay) {
    if (needsDisplay) {
        this->dirtyRect = MakeRect(0, 0, this->frame.size.width, this->frame.size.height);
        this->dirty = true;
    } else {
        this->dirty = false;
    }
}

void Window::setNeedsDisplayInRect(Rect rect, View *view) {
    View *superview = view; // will become superview shortly
    do {
        superview = superview->superview;
        if (superview == NULL) break;
        rect.origin.x += superview->frame.origin.x;
        rect.origin.y += superview->frame.origin.y;
    } while (true);

    Rect finalRect;
    if (this->dirty) {
        finalRect = MakeRect(min(this->dirtyRect.origin.x, rect.origin.x), min(this->dirtyRect.origin.y, rect.origin.y), 0, 0);
        finalRect.size.width = max(this->dirtyRect.origin.x + this->dirtyRect.size.width, rect.origin.x + rect.size.width) - finalRect.origin.x;
        finalRect.size.height = max(this->dirtyRect.origin.y + this->dirtyRect.size.height, rect.origin.y + rect.size.height) - finalRect.origin.y;
    } else {
        finalRect = rect;
    }

    this->dirty = true;
    this->dirtyRect = finalRect;
}

Rect Window::getDirtyRect() {
    if (this->dirty) return this->dirtyRect;
    else return {0};
}

View* Window::getFocusedView() {
    return this->focusedView;
}