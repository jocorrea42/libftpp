#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "design_patterns/memento.hpp"
#include "design_patterns/observer.hpp"
#include <string>
#include <functional>

enum class WidgetEvent {
    WIDGET_CREATED,
    WIDGET_UPDATED,
    WIDGET_DESTROYED
};

class Widget : public Memento {
private:
    std::string _name;
    bool _isVisible;
    Observer<WidgetEvent, std::string> _widgetObserver;

    void _saveToSnapshot(Memento::Snapshot& snapshot) const override;
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override;

public:
    Widget(const std::string& name);
    virtual ~Widget();
    
    virtual void update();
    virtual void render();
    
    void show();
    void hide();
    
    std::string getName() const;
    bool isVisible() const;
    
    void subscribeToWidgetEvent(WidgetEvent event, const std::function<void(const std::string&)>& callback);
};

#endif // WIDGET_HPP