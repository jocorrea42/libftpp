#ifndef PERSISTENT_WORKER_HPP
#define PERSISTENT_WORKER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <unordered_map>
#include <string>
#include <functional>
#include <chrono>

class PersistentWorker {
private:
    std::atomic<bool> running_;
    std::thread worker_thread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::unordered_map<std::string, std::function<void()>> tasks_;

    void workerLoop();

public:
    PersistentWorker();
    ~PersistentWorker();

    void addTask(const std::string& name, const std::function<void()>& jobToExecute);
    void removeTask(const std::string& name);
};

#endif