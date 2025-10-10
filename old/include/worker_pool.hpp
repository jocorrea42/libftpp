#ifndef WORKER_POOL_HPP
#define WORKER_POOL_HPP


#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>


namespace ft {


class WorkerPool {
public:
class IJob {
public:
virtual void execute() = 0;
virtual ~IJob() = default;
};


WorkerPool(size_t numWorkers = std::thread::hardware_concurrency());
~WorkerPool();


void addJob(const std::function<void()>& jobToExecute);


private:
std::vector<std::thread> _workers;
std::queue<std::function<void()>> _jobs;
std::mutex _mutex;
std::condition_variable _cv;
std::atomic<bool> _running{true};


void workerLoop();
};


} // namespace ft


#endif // WORKER_POOL_HPP