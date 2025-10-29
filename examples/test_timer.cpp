#include "bonus/timer.hpp"
#include "iostreams/thread_safe_iostream.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    Timer* timer = Timer::instance();
    
    timer->subscribeToTimerEvent(TimerEvent::TIMER_STARTED, [](const std::string& msg) {
        threadSafeCout << "[TIMER EVENT] " << msg << std::endl;
    });
    
    timer->subscribeToTimerEvent(TimerEvent::TIMER_EXPIRED, [](const std::string& msg) {
        threadSafeCout << "[TIMER EVENT] " << msg << std::endl;
    });
    
    // Test 1: Timer de 2 segundos
    threadSafeCout << "=== Test 1: Timer de 2 segundos ===" << std::endl;
    timer->setDuration(std::chrono::seconds(2));
    timer->start();
    
    while (!timer->hasExpired()) {
        threadSafeCout << "Tiempo restante: " << timer->getRemainingTime().count() << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Test 2: Timer con pausa
    threadSafeCout << "\n=== Test 2: Timer con pausa ===" << std::endl;
    timer->setDuration(std::chrono::seconds(3));
    timer->start();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer->pause();
    threadSafeCout << "Timer pausado. Tiempo restante: " << timer->getRemainingTime().count() << "ms" << std::endl;
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    timer->resume();
    threadSafeCout << "Timer reanudado" << std::endl;
    
    while (!timer->hasExpired()) {
        threadSafeCout << "Tiempo restante: " << timer->getRemainingTime().count() << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    threadSafeCout << "✅ Todos los tests del Timer completados exitosamente!" << std::endl;
    return 0;
}