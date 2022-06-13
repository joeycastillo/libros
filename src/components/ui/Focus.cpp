#include "Focus.h"
#include "Arduino.h"
#include <algorithm>

Task::Task() {
}

View::View(int16_t x, int16_t y, int16_t width, int16_t height) {
    Serial.print("Creating view ");
    Serial.println((int32_t) this);
    this->frame = MakeRect(x, y, width, height);
    this->window.reset();
    this->superview.reset();
}

View::~View() {
    Serial.print("Destroying view ");
    Serial.println((int32_t) this);
}

void View::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    Serial.print("Drawing view ");
    Serial.println((int32_t) this);
    if (this->opaque || this->backgroundColor) {
        Serial.print("Drawing fill ");
        Serial.println((int32_t) this);
        display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->backgroundColor);
    }
    for(std::shared_ptr<View> view : this->subviews) {
        view->draw(display, this->frame.origin.x, this->frame.origin.y);
    }
}

void View::addSubview(std::shared_ptr<View> view) {
    view->superview = this->shared_from_this();
    this->subviews.push_back(view);
    if (std::shared_ptr<Window> window = this->window.lock()) {
        view->window = window;
        window->setNeedsDisplay(true);
    }
}

void View::removeSubview(std::shared_ptr<View> view) {
    view->superview.reset();
    view->window.reset();
    int index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), view));
    this->subviews.erase(this->subviews.begin() + index);
    if (std::shared_ptr<Window> window = this->window.lock()) {
        window->setNeedsDisplay(true);
    }
}

void View::becomeFocused() {
    if (std::shared_ptr<Window> window = this->window.lock()) {
        std::shared_ptr<View> oldResponder = window->focusedView.lock();
        if (oldResponder != NULL) {
            oldResponder->willResignFocus();
            window->focusedView.reset();
            oldResponder->didResignFocus();
        }
        this->willBecomeFocused();
        window->focusedView = this->shared_from_this();
        this->didBecomeFocused();
    }
}

void View::resignFocus() {
    if (std::shared_ptr<Window> window = this->window.lock()) {
        if (std::shared_ptr<View> superview = this->superview.lock()) {
            superview->becomeFocused();
        }
    }
}

void View::movedToWindow() {
    // nothing to do here
}

bool View::canBecomeFocused() {
    return false;
}

void View::willBecomeFocused() {
    // nothing to do here
}

void View::didBecomeFocused() {
    if (this->superview.lock()) {
        if (std::shared_ptr<Window> window = this->window.lock()) {
            std::shared_ptr<View> shared_this = this->shared_from_this();
            window->setNeedsDisplayInRect(this->frame, shared_this);
        }
    }
}

void View::willResignFocus() {
    // nothing to do here
}

void View::didResignFocus() {
    if (this->superview.lock()) {
        if (std::shared_ptr<Window> window = this->window.lock()) {
            std::shared_ptr<View> shared_this = this->shared_from_this();
            window->setNeedsDisplayInRect(this->frame, shared_this);
        }
    }
}

bool View::handleEvent(Event event) {
    std::shared_ptr<View> focusedView = NULL;
    std::shared_ptr<Window> window = NULL;
    if (window = this->window.lock()) {
        focusedView = window->getFocusedView().lock();
    } else {
        focusedView = this->shared_from_this();
        if (focusedView == NULL) return false;
        window = std::static_pointer_cast<Window, View>(focusedView);
    }

    if (this->actions.count(event.type)) {
        if (std::shared_ptr<Application> application = window->application.lock()) {
            this->actions[event.type](application, event);
        }
    } else if (event.type < BUTTON_CENTER) {
        uint32_t index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
        if (this->affinity == DirectionalAffinityVertical) {
            switch (event.type) {
                case BUTTON_UP:
                    while (index > 0) {
                        if (this->subviews[index - 1]->canBecomeFocused()) this->subviews[index - 1]->becomeFocused();
                        else index--;
                        return true;
                    }
                    break;
                case BUTTON_DOWN:
                    while ((index + 1) < this->subviews.size()) {
                        if (this->subviews[index + 1]->canBecomeFocused()) this->subviews[index + 1]->becomeFocused();
                        else index--;
                        return true;
                    }
                    break;
                default:
                    break;
            }
        } else if (this->affinity == DirectionalAffinityHorizontal) {
            switch (event.type) {
                case BUTTON_LEFT:
                    while (index > 0) {
                        if (this->subviews[index - 1]->canBecomeFocused()) this->subviews[index - 1]->becomeFocused();
                        return true;
                    }
                    break;
                case BUTTON_RIGHT:
                    while ((index + 1) < this->subviews.size()) {
                        if (this->subviews[index + 1]->canBecomeFocused()) this->subviews[index + 1]->becomeFocused();
                        return true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (std::shared_ptr<View> superview = this->superview.lock()) {
        superview->handleEvent(event);
    }

    return false;
}

void View::setAction(Action action, EventType type) {
    this->actions[type] = action;
}

void View::removeAction(EventType type) {
    // TODO: remove the action
}

std::weak_ptr<View> View::getSuperview() {
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
        std::shared_ptr<View> shared_this = this->shared_from_this();
        window->setNeedsDisplayInRect(dirtyRect, shared_this);
    }
}

bool View::isOpaque() {
    return this->opaque;
}

void View::setOpaque(bool value) {
    this->opaque = value;
}

uint16_t View::getBackgroundColor() {
    return this->backgroundColor;
}

void View::setBackgroundColor(uint16_t value) {
    this->backgroundColor = value;
}

Control::Control(int16_t x, int16_t y, int16_t width, int16_t height) : View(x, y, width, height) {
}

bool Control::isEnabled() {
    return this->enabled;
}

void Control::setEnabled(bool value) {
    this->enabled = value;
}

bool Control::canBecomeFocused() {
    return this->enabled;
}


Application::Application(const std::shared_ptr<Window>& window) {
    this->window = window;
}

void Application::addTask(std::shared_ptr<Task> task) {
    this->tasks.push_back(task);
}

void Application::run() {
    this->window->application = this->shared_from_this();
    this->window->becomeFocused();
    this->window->setNeedsDisplay(true);
    while(true) {
        for(std::shared_ptr<Task> task : this->tasks) {
            if (task->run(this) != 0) return;
        }
    }
}

void Application::generateEvent(EventType eventType, int32_t userInfo) {
    Event event;
    event.type = eventType;
    event.userInfo = userInfo;
    if (std::shared_ptr<View> focusedView = this->window->focusedView.lock()) {
        focusedView->handleEvent(event);
    }
}

std::shared_ptr<Window> Application::getWindow() {
    return this->window;
}

Window::Window(int16_t width, int16_t height) : View(0, 0, width, height) {
    this->dirtyRect = MakeRect(0, 0, width, height);
}

void Window::addSubview(std::shared_ptr<View> view) {
    view->window = std::static_pointer_cast<Window>(this->shared_from_this());
    View::addSubview(view);
}

bool Window::canBecomeFocused() {
    return true;
}

void Window::becomeFocused() {
    if (!this->canBecomeFocused()) return;

    std::shared_ptr<View> oldResponder = this->focusedView.lock();
    if (oldResponder != NULL) {
        oldResponder->willResignFocus();
        this->focusedView.reset();
        oldResponder->didResignFocus();
    }
    this->willBecomeFocused();
    this->focusedView = this->shared_from_this();
    this->didBecomeFocused();
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

void Window::setNeedsDisplayInRect(Rect rect, std::shared_ptr<View> view) {
    std::shared_ptr<View> superview(view);
    while(superview = superview->superview.lock()) {
        rect.origin.x += superview->frame.origin.x;
        rect.origin.y += superview->frame.origin.y;
    }

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

std::weak_ptr<View> Window::getFocusedView() {
    return this->focusedView;
}
