#include "Focus.h"
#include "Adafruit_EPD.h"
#include <algorithm>

Task::Task() {
}


View::View(int16_t x, int16_t y, int16_t width, int16_t height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->window = NULL;
    this->superview = NULL;
}

void View::draw(Adafruit_GFX *display, int16_t x, int16_t y) {
    display->drawRect(x + this->x, y + this->y, this->width, this->height, EPD_BLACK);
    for(View *view : this->subviews) {
        view->draw(display, this->x, this->y);
    }
}

void View::addSubview(View *view) {
    Serial.println("Adding subview. New vec size:");
    view->superview = this;
    view->window = this->window;
    this->subviews.push_back(view);
    this->window->setNeedsDisplay(true);
    Serial.println(this->subviews.size());
}

void View::removeSubview(View *view) {
    Serial.println("Removing subview. New vec size:");
    view->superview = NULL;
    view->window = NULL;
    int index = std::distance(this->subviews.begin(), std::find(this->subviews.begin(), this->subviews.end(), view));
    this->subviews.erase(this->subviews.begin() + index);
    this->window->setNeedsDisplay(true);
    Serial.println(this->subviews.size());
}

// ADD MORE HERE

void View::handleEvent(Event event) {
    if (this->actions.count(event.type)) {
        this->actions[event.type](event);
    }
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

Window::Window(int16_t x, int16_t y, int16_t width, int16_t height) : View(x,  y, width, height) {
    this->focusedView = this;
    this->window = this;
    this->dirtyRect = MakeRect(0, 0, width, height);
}
void Window::setFocusTargets(View *view, View *up, View *right, View *down, View *left) {
    // TODO
}

bool Window::needsDisplay() {
    return (this->dirtyRect.size.width > 0 && this->dirtyRect.size.height > 0);
}

void Window::setNeedsDisplay(bool needsDisplay) {
    if (needsDisplay) {
        this->dirtyRect = MakeRect(0, 0, width, height);
    } else {
        this->dirtyRect = MakeRect(0, 0, 0, 0);
    }
}
