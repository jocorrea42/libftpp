#include "bonus/timer.hpp"
#include <chrono>

Timer::Timer() : _duration(0), _remaining(0), _isRunning(false), _isPaused(false) {}

void Timer::setDuration(const std::chrono::milliseconds& duration) {
    _duration = duration;
    _remaining = duration;
}

void Timer::start() {
    _startTime = std::chrono::steady_clock::now();
    _isRunning = true;
    _isPaused = false;
    _remaining = _duration;
    _timerObserver.notify(TimerEvent::TIMER_STARTED, "Timer started");
}

void Timer::pause() {
    if (_isRunning && !_isPaused) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime);
        _remaining = _duration > elapsed ? _duration - elapsed : std::chrono::milliseconds(0);
        _isPaused = true;
        _timerObserver.notify(TimerEvent::TIMER_PAUSED, "Timer paused");
    }
}

void Timer::resume() {
    if (_isRunning && _isPaused) {
        _startTime = std::chrono::steady_clock::now();
        _isPaused = false;
        _timerObserver.notify(TimerEvent::TIMER_RESUMED, "Timer resumed");
    }
}

void Timer::stop() {
    _isRunning = false;
    _isPaused = false;
    _timerObserver.notify(TimerEvent::TIMER_STOPPED, "Timer stopped");
}

bool Timer::hasExpired() const {
    if (!_isRunning || _isPaused) return false;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime);
    
    bool expired = elapsed >= _duration;
    if (expired && _isRunning) {
        const_cast<Timer*>(this)->_timerObserver.notify(TimerEvent::TIMER_EXPIRED, "Timer expired");
        const_cast<Timer*>(this)->_isRunning = false;
    }
    
    return expired;
}

std::chrono::milliseconds Timer::getRemainingTime() const {
    if (!_isRunning) return _remaining;
    if (_isPaused) return _remaining;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime);
    return _duration > elapsed ? _duration - elapsed : std::chrono::milliseconds(0);
}

void Timer::subscribeToTimerEvent(TimerEvent event, const std::function<void(const std::string&)>& callback) {
    _timerObserver.subscribe(event, callback);
}

bool Timer::isRunning() const { return _isRunning; }
bool Timer::isPaused() const { return _isPaused; }