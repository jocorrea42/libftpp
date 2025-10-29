#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "design_patterns/singleton.hpp"
#include "design_patterns/observer.hpp"
#include "design_patterns/state_machine.hpp"
#include <vector>
#include <memory>
#include <string>

enum class AppState {
    INITIALIZING,
    RUNNING,
    PAUSED,
    SHUTTING_DOWN
};

enum class AppEvent {
    STARTED,
    PAUSED,
    RESUMED,
    STOPPED,
    WIDGET_ADDED,
    WIDGET_REMOVED
};

class Widget;

class Application : public Singleton<Application> {
private:
    friend class Singleton<Application>;
    
    std::vector<std::shared_ptr<Widget>> _widgets;
    StateMachine<AppState> _stateMachine;
    Observer<AppEvent, std::string> _appObserver;
    bool _isInitialized;

    Application();

public:
    void initialize();
    void run();
    void pause();
    void resume();
    void shutdown();
    
    void addWidget(const std::shared_ptr<Widget>& widget);
    void removeWidget(const std::string& widgetName);
    
    void subscribeToAppEvent(AppEvent event, const std::function<void(const std::string&)>& callback);
    AppState getCurrentState() const;
    size_t getWidgetCount() const;
};

#endif // APPLICATION_HPP