#include "threading/worker_pool.hpp"

WorkerPool::WorkerPool(size_t poolSize) : running(true) {
    for (size_t i = 0; i < poolSize; i++) {
        threads.push_back(std::thread(&WorkerPool::loop, this));
    }
}

WorkerPool::~WorkerPool() {
    running = false;
    cv.notify_all();
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void WorkerPool::loop() {
    while (running) {
        std::shared_ptr<IJob> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !jobs.empty() || !running; });

            if (!running)
                return;

            task = jobs.front();
            jobs.pop();
        }
        task->execute();
        task = nullptr;
    }
}

void WorkerPool::addJob(const std::shared_ptr<IJob>& jobToExecute) {
    if (!jobToExecute)
        return;
    std::lock_guard<std::mutex> lock(mtx);
    jobs.push(jobToExecute);
    cv.notify_one();
}

void WorkerPool::addJob(const Callback& jobToExecute) {
    auto job = std::make_shared<FunctionJob>(jobToExecute);
    std::lock_guard<std::mutex> lock(mtx);
    jobs.push(job);
    cv.notify_one();
}

/* FunctionJob */

FunctionJob::FunctionJob(const Callback& jobToExecute) : job(jobToExecute) {}

void FunctionJob::execute() {
    if (job)
        job();
}