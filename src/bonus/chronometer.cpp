#include "bonus/chronometer.hpp"
#include "iostreams/thread_safe_iostream.hpp"

Chronometer::Chronometer() : _isRunning(false) {}

void Chronometer::_saveToSnapshot(Memento::Snapshot& snapshot) const {
    snapshot << _isRunning;
    snapshot << static_cast<uint64_t>(_laps.size());
    for (const auto& lap : _laps) {
        snapshot << lap.count();
    }
}

void Chronometer::_loadFromSnapshot(Memento::Snapshot& snapshot) {
    snapshot >> _isRunning;
    uint64_t lapCount;
    snapshot >> lapCount;
    _laps.clear();
    for (uint64_t i = 0; i < lapCount; ++i) {
        double lapTime;
        snapshot >> lapTime;
        _laps.push_back(std::chrono::duration<double>(lapTime));
    }
}

void Chronometer::start() {
    _startTime = std::chrono::steady_clock::now();
    _isRunning = true;
    threadSafeCout << "Chronometer started" << std::endl;
}

void Chronometer::stop() {
    if (_isRunning) {
        _endTime = std::chrono::steady_clock::now();
        _isRunning = false;
        threadSafeCout << "Chronometer stopped. Total: " << getElapsedTime().count() << "s" << std::endl;
    }
}

void Chronometer::lap() {
    if (_isRunning) {
        auto current = std::chrono::steady_clock::now();
        auto lapTime = current - _startTime;
        _laps.push_back(lapTime);
        
        threadSafeCout << "Lap " << _laps.size() << ": " 
                      << std::chrono::duration_cast<std::chrono::milliseconds>(lapTime).count() 
                      << "ms" << std::endl;
    }
}

std::chrono::duration<double> Chronometer::getElapsedTime() const {
    if (_isRunning) {
        return std::chrono::steady_clock::now() - _startTime;
    } else {
        return _endTime - _startTime;
    }
}

void Chronometer::reset() {
    _isRunning = false;
    _laps.clear();
    threadSafeCout << "Chronometer reset" << std::endl;
}

const std::vector<std::chrono::duration<double>>& Chronometer::getLaps() const {
    return _laps;
}

bool Chronometer::isRunning() const {
    return _isRunning;
}