#include "Focus.h"
#include <map>

Task::Task() {
}


View::View(int16_t x, int16_t y, int16_t width, int16_t height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->window = NULL;
    this->nextResponder = NULL;
}

void View::draw(int16_t x, int16_t y) {
}

void View::addSubview(View *view) {
    view->nextResponder = this;
    view->window = this->window;
}

void View::removeSubview(View *view) {

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

Window::Window(int16_t x, int16_t y, int16_t width, int16_t height) : View(x,  y, width, height) {
    this->focusedView = this;
    this->window = this;
}
