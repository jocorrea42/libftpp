#include "worker_pool.hpp"


namespace ft {


WorkerPool::WorkerPool(size_t numWorkers) {
for (size_t i = 0; i < numWorkers; ++i) {
_workers.emplace_back([this]() { workerLoop(); });
}
}


WorkerPool::~WorkerPool() {
_running = false;
_cv.notify_all();
for (auto &worker : _workers) {
if (worker.joinable()) worker.join();
}
}


void WorkerPool::addJob(const std::function<void()>& jobToExecute) {
{
std::lock_guard<std::mutex> lock(_mutex);
_jobs.push(jobToExecute);
}
_cv.notify_one();
}


void WorkerPool::workerLoop() {
while (_running) {
std::function<void()> job;
{
std::unique_lock<std::mutex> lock(_mutex);
_cv.wait(lock, [this]() { return !_jobs.empty() || !_running; });
if (!_running) break;
job = _jobs.front();
_jobs.pop();
}
job();
}
}


} // namespace ft