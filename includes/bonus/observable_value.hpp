#ifndef OBSERVABLE_VALUE_HPP
#define OBSERVABLE_VALUE_HPP

#include "design_patterns/observer.hpp"
#include "design_patterns/memento.hpp"
#include <functional>
#include <algorithm>

enum class ValueEvent {
    VALUE_CHANGED,
    VALUE_VALIDATED,
    VALUE_RESET
};

template<typename T>
class ObservableValue : public Memento {
private:
    T _value;
    T _defaultValue;
    Observer<ValueEvent, T> _valueObserver;
    std::function<bool(const T&)> _validator;

    void _saveToSnapshot(Memento::Snapshot& snapshot) const override;
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override;

public:
    ObservableValue();
    ObservableValue(const T& initialValue);
    
    void setValue(const T& newValue);
    void setValidator(const std::function<bool(const T&)>& validator);
    void reset();
    
    T getValue() const;
    void subscribeToValueChange(const std::function<void(const T&)>& callback);
    
    bool operator==(const ObservableValue<T>& other) const;
    bool operator!=(const ObservableValue<T>& other) const;
};

#include "observable_value.tpp"

#endif // OBSERVABLE_VALUE_HPP