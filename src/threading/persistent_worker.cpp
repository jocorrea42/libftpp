#include "threading/persistent_worker.hpp"
#include "iostreams/thread_safe_iostream.hpp"
#include <iostream>
#include <chrono>

PersistentWorker::PersistentWorker() 
    : running_(true),
      worker_thread_(&PersistentWorker::workerLoop, this) {
}

PersistentWorker::~PersistentWorker() {
    running_ = false;
    condition_.notify_all();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void PersistentWorker::workerLoop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Wait until we have tasks or we're stopping
        condition_.wait(lock, [this]() { 
            return !tasks_.empty() || !running_; 
        });

        if (!running_) {
            break;
        }

        // Execute all tasks
        for (const auto& task_pair : tasks_) {
            if (task_pair.second) {
                try {
                    task_pair.second();
                } catch (const std::exception& e) {
                    std::cerr << "Exception in task '" << task_pair.first << "': " << e.what() << std::endl;
                }
            }
        }

        lock.unlock();
        
        // Small delay to prevent CPU spinning but still execute frequently
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void PersistentWorker::addTask(const std::string& name, const std::function<void()>& jobToExecute) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_[name] = jobToExecute;
    condition_.notify_one();
}

void PersistentWorker::removeTask(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.erase(name);
}