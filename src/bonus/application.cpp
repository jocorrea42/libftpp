#include "bonus/application.hpp"
#include "bonus/widget.hpp"
#include "iostreams/thread_safe_iostream.hpp"

Application::Application() : _isInitialized(false) {
    _stateMachine.addState(AppState::INITIALIZING);
    _stateMachine.addState(AppState::RUNNING);
    _stateMachine.addState(AppState::PAUSED);
    _stateMachine.addState(AppState::SHUTTING_DOWN);
}

void Application::initialize() {
    if (!_isInitialized) {
        _isInitialized = true;
        _stateMachine.transitionTo(AppState::INITIALIZING);
        _appObserver.notify(AppEvent::STARTED, "Application initialized");
        threadSafeCout << "Application initialized successfully" << std::endl;
    }
}

void Application::run() {
    if (_isInitialized) {
        _stateMachine.transitionTo(AppState::RUNNING);
        _appObserver.notify(AppEvent::STARTED, "Application running");
        threadSafeCout << "Application started running" << std::endl;
    }
}

void Application::pause() {
    if (_stateMachine.getCurrentState() == AppState::RUNNING) {
        _stateMachine.transitionTo(AppState::PAUSED);
        _appObserver.notify(AppEvent::PAUSED, "Application paused");
        threadSafeCout << "Application paused" << std::endl;
    }
}

void Application::resume() {
    if (_stateMachine.getCurrentState() == AppState::PAUSED) {
        _stateMachine.transitionTo(AppState::RUNNING);
        _appObserver.notify(AppEvent::RESUMED, "Application resumed");
        threadSafeCout << "Application resumed" << std::endl;
    }
}

void Application::shutdown() {
    _stateMachine.transitionTo(AppState::SHUTTING_DOWN);
    _appObserver.notify(AppEvent::STOPPED, "Application shutdown");
    threadSafeCout << "Application shutdown" << std::endl;
}

void Application::addWidget(const std::shared_ptr<Widget>& widget) {
    _widgets.push_back(widget);
    _appObserver.notify(AppEvent::WIDGET_ADDED, "Widget added: " + widget->getName());
    threadSafeCout << "Widget added: " << widget->getName() << std::endl;
}

void Application::removeWidget(const std::string& widgetName) {
    auto it = std::remove_if(_widgets.begin(), _widgets.end(),
        [&](const std::shared_ptr<Widget>& widget) {
            return widget->getName() == widgetName;
        });
    
    if (it != _widgets.end()) {
        _widgets.erase(it, _widgets.end());
        _appObserver.notify(AppEvent::WIDGET_REMOVED, "Widget removed: " + widgetName);
        threadSafeCout << "Widget removed: " << widgetName << std::endl;
    }
}

void Application::subscribeToAppEvent(AppEvent event, const std::function<void(const std::string&)>& callback) {
    _appObserver.subscribe(event, callback);
}

AppState Application::getCurrentState() const {
    return _stateMachine.getCurrentState();
}

size_t Application::getWidgetCount() const {
    return _widgets.size();
}