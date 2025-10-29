#ifndef THREAD_SAFE_QUEUE_TPP
#define THREAD_SAFE_QUEUE_TPP

#include "thread_safe_queue.hpp"
#include <chrono>
#include <sstream>

template <typename TType>
ThreadSafeQueue<TType>::ThreadSafeQueue() : _closed(false) {
    initializeStateMachine();
}

template <typename TType>
ThreadSafeQueue<TType>::~ThreadSafeQueue() {
    std::lock_guard<std::mutex> lock(_mutex);
    _closed = true;
    _cv.notify_all();
}

template <typename TType>
void ThreadSafeQueue<TType>::initializeStateMachine() {
    // ✅ CORRECCIÓN: StateMachine::addState solo acepta 1 parámetro
    _stateMachine.addState(QueueState::EMPTY);
    _stateMachine.addState(QueueState::NORMAL); 
    _stateMachine.addState(QueueState::CLOSED);
    
    // ✅ CORRECCIÓN: Definir transiciones explícitas entre estados
    // EMPTY → NORMAL (cuando se añade un elemento)
    _stateMachine.addTransition(QueueState::EMPTY, QueueState::NORMAL, []() {});
    
    // NORMAL → EMPTY (cuando se remueve el último elemento)  
    _stateMachine.addTransition(QueueState::NORMAL, QueueState::EMPTY, []() {});
    
    // NORMAL → CLOSED (cuando se cierra la cola)
    _stateMachine.addTransition(QueueState::NORMAL, QueueState::CLOSED, []() {});
    
    // EMPTY → CLOSED (cuando se cierra la cola vacía)
    _stateMachine.addTransition(QueueState::EMPTY, QueueState::CLOSED, []() {});
    
    // CLOSED → EMPTY (no debería ocurrir normalmente, pero por completitud)
    _stateMachine.addTransition(QueueState::CLOSED, QueueState::EMPTY, []() {});
    
    // CLOSED → NORMAL (no debería ocurrir normalmente, pero por completitud)
    _stateMachine.addTransition(QueueState::CLOSED, QueueState::NORMAL, []() {});
    
    // ✅ StateMachine automáticamente establece el primer estado (EMPTY) como inicial
    // NO necesitamos llamar a setInitialState
}

template <typename TType>
void ThreadSafeQueue<TType>::updateState() {
    QueueState newState = _closed ? QueueState::CLOSED : 
                        (_queue.empty() ? QueueState::EMPTY : QueueState::NORMAL);
    
    // ✅ CORRECCIÓN: Solo transicionar si es posible y si el estado es diferente
    if (_stateMachine.canTransitionTo(newState) && 
        _stateMachine.getCurrentState() != newState) {
        _stateMachine.transitionTo(newState);
    }
}

// Función helper inline para evitar problemas ODR
template<typename T>
inline std::string valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Especialización inline para std::string
template<>
inline std::string valueToString<std::string>(const std::string& value) {
    return value;
}

template <typename TType>
void ThreadSafeQueue<TType>::push_back(const TType& newElement) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_closed) {
        throw std::runtime_error("Cannot push to closed queue");
    }
    
    _queue.push_back(newElement);
    _dataBuffer.append("Push back: " + valueToString(newElement) + "\n");
    updateState();
    
    _cv.notify_one();
    _eventObserver.notify(QueueEvent::ELEMENT_PUSHED);
}

template <typename TType>
void ThreadSafeQueue<TType>::push_front(const TType& newElement) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_closed) {
        throw std::runtime_error("Cannot push to closed queue");
    }
    
    _queue.push_front(newElement);
    _dataBuffer.append("Push front: " + valueToString(newElement) + "\n");
    updateState();
    
    _cv.notify_one();
    _eventObserver.notify(QueueEvent::ELEMENT_PUSHED);
}

template <typename TType>
TType ThreadSafeQueue<TType>::pop_back() {
    std::unique_lock<std::mutex> lock(_mutex);
    
    // Esperar con timeout
    if (!_cv.wait_for(lock, std::chrono::seconds(1), [this] { 
        return !_queue.empty() || _closed; 
    })) {
        throw std::runtime_error("Timeout: cannot pop from empty queue");
    }
    
    if (_queue.empty()) {
        throw std::runtime_error("Cannot pop from empty queue");
    }
    
    TType value = std::move(_queue.back());
    _queue.pop_back();
    _dataBuffer.append("Pop back: " + valueToString(value) + "\n");
    updateState();
    
    _eventObserver.notify(QueueEvent::ELEMENT_POPPED);
    return value;
}

template <typename TType>
TType ThreadSafeQueue<TType>::pop_front() {
    std::unique_lock<std::mutex> lock(_mutex);
    
    // Esperar con timeout
    if (!_cv.wait_for(lock, std::chrono::seconds(1), [this] { 
        return !_queue.empty() || _closed; 
    })) {
        throw std::runtime_error("Timeout: cannot pop from empty queue");
    }
    
    if (_queue.empty()) {
        throw std::runtime_error("Cannot pop from empty queue");
    }
    
    TType value = std::move(_queue.front());
    _queue.pop_front();
    _dataBuffer.append("Pop front: " + valueToString(value) + "\n");
    updateState();
    
    _eventObserver.notify(QueueEvent::ELEMENT_POPPED);
    return value;
}

template <typename TType>
bool ThreadSafeQueue<TType>::empty() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.empty();
}

template <typename TType>
size_t ThreadSafeQueue<TType>::size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.size();
}

template <typename TType>
void ThreadSafeQueue<TType>::_saveToSnapshot(Memento::Snapshot& snapshot) const {
    std::lock_guard<std::mutex> lock(_mutex);
    snapshot << _closed;
    snapshot.save_string(_dataBuffer.str());
    
    // Guardar el tamaño de la queue y sus elementos
    size_t size = _queue.size();
    snapshot << size;
    for (const auto& item : _queue) {
        snapshot << item;
    }
}

template <typename TType>
void ThreadSafeQueue<TType>::_loadFromSnapshot(Memento::Snapshot& snapshot) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    snapshot >> _closed;
    
    std::string bufferContents;
    snapshot.load_string(bufferContents);
    _dataBuffer.clear();
    _dataBuffer.append(bufferContents);
    
    // Cargar elementos de la queue
    size_t size;
    snapshot >> size;
    _queue.clear();
    for (size_t i = 0; i < size; ++i) {
        TType item;
        snapshot >> item;
        _queue.push_back(item);
    }
    
    updateState(); // ✅ Actualizar estado después de restaurar
    _cv.notify_all();
}

#endif // THREAD_SAFE_QUEUE_TPP