#include "bonus/widget.hpp"
#include "iostreams/thread_safe_iostream.hpp"

Widget::Widget(const std::string& name) : _name(name), _isVisible(true) {
    _widgetObserver.notify(WidgetEvent::WIDGET_CREATED, "Widget created: " + _name);
    threadSafeCout << "Widget created: " << _name << std::endl;
}

Widget::~Widget() {
    _widgetObserver.notify(WidgetEvent::WIDGET_DESTROYED, "Widget destroyed: " + _name);
    threadSafeCout << "Widget destroyed: " << _name << std::endl;
}

void Widget::_saveToSnapshot(Memento::Snapshot& snapshot) const {
    snapshot << _name;
    snapshot << _isVisible;
}

void Widget::_loadFromSnapshot(Memento::Snapshot& snapshot) {
    snapshot >> _name;
    snapshot >> _isVisible;
}

void Widget::update() {
    _widgetObserver.notify(WidgetEvent::WIDGET_UPDATED, "Widget updated: " + _name);
    threadSafeCout << "Widget updated: " << _name << std::endl;
}

void Widget::render() {
    if (_isVisible) {
        threadSafeCout << "Rendering widget: " << _name << std::endl;
    }
}

void Widget::show() {
    _isVisible = true;
    threadSafeCout << "Widget shown: " << _name << std::endl;
}

void Widget::hide() {
    _isVisible = false;
    threadSafeCout << "Widget hidden: " << _name << std::endl;
}

std::string Widget::getName() const {
    return _name;
}

bool Widget::isVisible() const {
    return _isVisible;
}

void Widget::subscribeToWidgetEvent(WidgetEvent event, const std::function<void(const std::string&)>& callback) {
    _widgetObserver.subscribe(event, callback);
}