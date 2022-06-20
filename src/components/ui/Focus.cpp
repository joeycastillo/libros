#include "Focus.h"
#include "Arduino.h"
#include <algorithm>

Task::Task() {
}

View::View(Rect rect) {
    // Serial.print("Creating view ");
    // Serial.println((int32_t) this);
    this->frame = rect;
    this->window.reset();
    this->superview.reset();
}

View::~View() {
    // Serial.print("Destroying view ");
    // Serial.println((int32_t) this);
}

void View::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    // Serial.print("Drawing view ");
    // Serial.println((int32_t) this);
    if (this->opaque || this->backgroundColor) {
        display->fillRect(x + this->frame.origin.x, y + this->frame.origin.y, this->frame.size.width, this->frame.size.height, this->backgroundColor);
    }
    for(std::shared_ptr<View> view : this->subviews) {
        view->draw(display, this->frame.origin.x, this->frame.origin.y);
    }
}

void View::addSubview(std::shared_ptr<View> view) {
    view->superview = this->shared_from_this();
    this->subviews.push_back(view);
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        view->setWindow(window);
        window->setNeedsDisplay(true);
    }
}

void View::removeSubview(std::shared_ptr<View> view) {
    if (view->isFocused()) {
        view->resignFocus();
    }
    view->superview.reset();
    view->window.reset();
    int index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), view));
    this->subviews.erase(this->subviews.begin() + index);    
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        // FIXME: We should only refocus if we know the focused view was removed.
        window->becomeFocused();
        window->setNeedsDisplay(true);
    }
}

bool View::isFocused() {
    return this->focused;
}

bool View::canBecomeFocused() {
    return false;
}

bool View::becomeFocused() {
    for(std::shared_ptr<View> subview : this->subviews) {
        if (subview->becomeFocused()) {
            return true;
        }
    }

    if (this->canBecomeFocused()) {
        // when there are no focusable subviews, and we can become
        // focused, become focused ourselves.
        // note that Window can always become focused, so this
        // block is guaranteed to execute when we reach the window.
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
            std::shared_ptr<View> oldResponder = window->getFocusedView().lock();
            if (oldResponder != NULL) {
                oldResponder->willResignFocus();
                oldResponder->focused = false;
                window->focusedView.reset();
                oldResponder->didResignFocus();
            }
            this->willBecomeFocused();
            this->focused = true;
            window->focusedView = this->shared_from_this();
            this->didBecomeFocused();
        }

        return true;
    }

    return false;
}

void View::resignFocus() {
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
        if (std::shared_ptr<View> superview = this->superview.lock()) {
            superview->becomeFocused();
        }
    }
}

void View::movedToWindow() {
    // nothing to do here
}

void View::willBecomeFocused() {
    // nothing to do here
}

void View::didBecomeFocused() {
    if (this->superview.lock()) {
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
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
        if (std::shared_ptr<Window> window = this->getWindow().lock()) {
            std::shared_ptr<View> shared_this = this->shared_from_this();
            window->setNeedsDisplayInRect(this->frame, shared_this);
        }
    }
}

bool View::handleEvent(Event event) {
    std::shared_ptr<View> focusedView = NULL;
    std::shared_ptr<Window> window = NULL;
    if (window = this->getWindow().lock()) {
        focusedView = window->getFocusedView().lock();
    } else {
        focusedView = this->shared_from_this();
        if (focusedView == NULL) return false;
        window = std::static_pointer_cast<Window, View>(focusedView);
    }

    if (this->actions.count(event.type)) {
        if (std::shared_ptr<Application> application = window->application.lock()) {
            this->actions[event.type](event);
        }
    } else if (event.type < FOCUS_EVENT_BUTTON_CENTER) {
        uint32_t index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), focusedView));
        if (this->affinity == DirectionalAffinityVertical) {
            switch (event.type) {
                case FOCUS_EVENT_BUTTON_UP:
                    while (index > 0) {
                        if (this->subviews[index - 1]->canBecomeFocused()) this->subviews[index - 1]->becomeFocused();
                        else index--;
                        return true;
                    }
                    break;
                case FOCUS_EVENT_BUTTON_DOWN:
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
                case FOCUS_EVENT_BUTTON_LEFT:
                    while (index > 0) {
                        if (this->subviews[index - 1]->canBecomeFocused()) this->subviews[index - 1]->becomeFocused();
                        return true;
                    }
                    break;
                case FOCUS_EVENT_BUTTON_RIGHT:
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

void View::setAction(const Action &action, int32_t type) {
    this->actions[type] = action;
}

void View::removeAction(int32_t type) {
    // TODO: remove the action
}

std::weak_ptr<View> View::getSuperview() {
    return this->superview;
}

std::weak_ptr<Window> View::getWindow() {
    return this->window;
}

void View::setWindow(std::shared_ptr<Window>window) {
    this->window = window;
    for(std::shared_ptr<View> subview : this->subviews) {
        subview->setWindow(window);
    }
}

Rect View::getFrame() {
    return this->frame;
}

void View::setFrame(Rect frame) {
    if (std::shared_ptr<Window> window = this->getWindow().lock()) {
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

Control::Control(Rect rect) : View(rect) {
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

Window::Window(Size size) : View(MakeRect(0, 0, size.width, size.height)) {
    this->setNeedsDisplay(true);
}

void Window::addSubview(std::shared_ptr<View> view) {
    view->setWindow(std::static_pointer_cast<Window>(this->shared_from_this()));
    View::addSubview(view);
    // when we add a new view hierarchy to the window, try to focus on its innermost view.
    this->becomeFocused();
}

bool Window::canBecomeFocused() {
    return true;
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

std::weak_ptr<View> Window::getSuperview() {
    return std::weak_ptr<View>();
}

std::weak_ptr<Window> Window::getWindow() {
    return std::static_pointer_cast<Window, View>(this->shared_from_this());
}

void Window::setWindow(std::shared_ptr<Window> window) {
    // nothing to do here
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

void Application::generateEvent(int32_t eventType, int32_t userInfo) {
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

void Application::setRootViewController(std::shared_ptr<ViewController> viewController) {
    if (this->rootViewController) {
        // clean up old view controller
        this->rootViewController->viewWillDisappear();
        this->window->removeSubview(this->rootViewController->view);
        this->rootViewController->viewDidDisappear();
    }

    // set up new view controller
    this->rootViewController = viewController;
    this->rootViewController->viewWillAppear();
    this->window->addSubview(this->rootViewController->view);
    this->rootViewController->viewDidAppear();
}

void ViewController::viewWillAppear() {
    if (!this->view) {
        this->createView();
    }
}

void ViewController::viewDidDisappear() {
    this->destroyView();
}

void ViewController::generateEvent(int32_t eventType, int32_t userInfo) {
    if (!this->view) return;

    // unsure about this one: we generate an event and let it bubble up to the window,
    // where the application can listen for it. seems like wasted effort to get a message
    // from a view controller to the application.
    if (std::shared_ptr<Window> window = this->view->getWindow().lock()) {
        if (std::shared_ptr<Application> application = window->application.lock()) {
            application->generateEvent(eventType, userInfo);
        }
    }
}

void ViewController::createView() {
    if (this->view) {
        this->destroyView();
    }
    // subclasses must override to create view here
}

void ViewController::destroyView() {
    this->view.reset();
}
