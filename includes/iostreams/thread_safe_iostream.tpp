#ifndef THREAD_SAFE_IOSTREAM_TPP
#define THREAD_SAFE_IOSTREAM_TPP

#include "thread_safe_iostream.hpp"

template<typename T>
ThreadSafeIOStream& ThreadSafeIOStream::operator<<(const T& value) {
    getLocalBuffer() << value;
    if (_stateMachine.getCurrentState() == StreamState::IDLE) {
        _stateMachine.transitionTo(StreamState::BUFFERING);
    }
    return *this;
}

template<typename T>
ThreadSafeIOStream& ThreadSafeIOStream::operator>>(T& value) {
    std::lock_guard<std::mutex> lock(_cinMutex);
    std::cin >> value;
    return *this;
}

template<typename T>
void ThreadSafeIOStream::prompt(const std::string& question, T& dest) {
    {
        std::lock_guard<std::mutex> lock(_coutMutex);
        std::cout << getLocalPrefix() << question;
        _observer.notify(StreamEvent::LINE_PRINTED, "Prompt: " + question);
    }
    {
        std::lock_guard<std::mutex> lock(_cinMutex);
        std::cin >> dest;
    }
    _dataBuffer << "[PROMPT] " << question << " -> " << dest;
}

#endif // THREAD_SAFE_IOSTREAM_TPP