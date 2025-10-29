#ifndef TIMER_HPP
#define TIMER_HPP

#include "design_patterns/singleton.hpp"
#include "design_patterns/observer.hpp"
#include <chrono>
#include <functional>

enum class TimerEvent {
    TIMER_STARTED,
    TIMER_PAUSED,
    TIMER_RESUMED,
    TIMER_EXPIRED,
    TIMER_STOPPED
};

class Timer : public Singleton<Timer> {
private:
    friend class Singleton<Timer>;
    
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::milliseconds _duration;
    std::chrono::milliseconds _remaining;
    bool _isRunning;
    bool _isPaused;
    
    Observer<TimerEvent, std::string> _timerObserver;

    Timer();

public:
    void setDuration(const std::chrono::milliseconds& duration);
    void start();
    void pause();
    void resume();
    void stop();
    bool hasExpired() const;
    std::chrono::milliseconds getRemainingTime() const;
    void subscribeToTimerEvent(TimerEvent event, const std::function<void(const std::string&)>& callback);
    bool isRunning() const;
    bool isPaused() const;
};

#endif // TIMER_HPP