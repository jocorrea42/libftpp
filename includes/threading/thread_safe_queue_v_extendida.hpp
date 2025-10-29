#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include "data_structures/data_buffer.hpp"
#include "design_patterns/memento.hpp"
#include "design_patterns/singleton.hpp"
#include "design_patterns/state_machine.hpp"
#include "design_patterns/observer.hpp"

// Estados para la State Machine
enum class QueueState {
    EMPTY,
    NORMAL,
    CLOSED
};

// Eventos para el Observer
enum class QueueEvent {
    ELEMENT_PUSHED,
    ELEMENT_POPPED
};

template <typename TType>
class ThreadSafeQueue : public Memento {
private:
    std::deque<TType> _queue;
    mutable std::mutex _mutex;
    std::condition_variable _cv;
    bool _closed;
    
    // Patrones integrados
    StateMachine<QueueState> _stateMachine;
    Observer<QueueEvent, void> _eventObserver;
    DataBuffer _dataBuffer;

public:
    ThreadSafeQueue();
    ~ThreadSafeQueue();

    // Métodos requeridos
    void push_back(const TType& newElement);
    void push_front(const TType& newElement);
    TType pop_back();
    TType pop_front();

    // Métodos de consulta
    bool empty() const;
    size_t size() const;

private:
    // Implementaciones Memento
    void _saveToSnapshot(Memento::Snapshot& snapshot) const override;
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override;
    
    void initializeStateMachine();
    void updateState();
};

#include "thread_safe_queue.tpp"

#endif