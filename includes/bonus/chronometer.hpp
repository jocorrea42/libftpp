#ifndef CHRONOMETER_HPP
#define CHRONOMETER_HPP

#include "design_patterns/memento.hpp"
#include <chrono>
#include <vector>

class Chronometer : public Memento {
private:
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _endTime;
    bool _isRunning;
    std::vector<std::chrono::duration<double>> _laps;
    
    void _saveToSnapshot(Memento::Snapshot& snapshot) const override;
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override;

public:
    Chronometer();
    void start();
    void stop();
    void lap();
    void reset();
    std::chrono::duration<double> getElapsedTime() const;
    const std::vector<std::chrono::duration<double>>& getLaps() const;
    bool isRunning() const;
};

#endif // CHRONOMETER_HPP