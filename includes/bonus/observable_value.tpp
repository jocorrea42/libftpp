#ifndef OBSERVABLE_VALUE_TPP
#define OBSERVABLE_VALUE_TPP

#include "iostreams/thread_safe_iostream.hpp"

template<typename T>
ObservableValue<T>::ObservableValue() : _value(T()), _defaultValue(T()) {}

template<typename T>
ObservableValue<T>::ObservableValue(const T& initialValue) 
    : _value(initialValue), _defaultValue(initialValue) {}

template<typename T>
void ObservableValue<T>::_saveToSnapshot(Memento::Snapshot& snapshot) const {
    snapshot << _value;
    snapshot << _defaultValue;
}

template<typename T>
void ObservableValue<T>::_loadFromSnapshot(Memento::Snapshot& snapshot) {
    snapshot >> _value;
    snapshot >> _defaultValue;
}

template<typename T>
void ObservableValue<T>::setValue(const T& newValue) {
    if (_validator && !_validator(newValue)) {
        threadSafeCout << "Value validation failed" << std::endl;
        return;
    }
    
    if (_value != newValue) {
        _value = newValue;
        _valueObserver.notify(ValueEvent::VALUE_CHANGED, _value);
        threadSafeCout << "Value changed to: " << newValue << std::endl;
    }
}

template<typename T>
void ObservableValue<T>::setValidator(const std::function<bool(const T&)>& validator) {
    _validator = validator;
}

template<typename T>
void ObservableValue<T>::reset() {
    _value = _defaultValue;
    _valueObserver.notify(ValueEvent::VALUE_RESET, _value);
    threadSafeCout << "Value reset to default: " << _defaultValue << std::endl;
}

template<typename T>
T ObservableValue<T>::getValue() const {
    return _value;
}

template<typename T>
void ObservableValue<T>::subscribeToValueChange(const std::function<void(const T&)>& callback) {
    _valueObserver.subscribe(ValueEvent::VALUE_CHANGED, callback);
}

template<typename T>
bool ObservableValue<T>::operator==(const ObservableValue<T>& other) const {
    return _value == other._value;
}

template<typename T>
bool ObservableValue<T>::operator!=(const ObservableValue<T>& other) const {
    return !(*this == other);
}

#endif // OBSERVABLE_VALUE_TPP