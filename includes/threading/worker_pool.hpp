#ifndef WORKER_POOL_HPP
# define WORKER_POOL_HPP

# include <thread>
# include <mutex>
# include <condition_variable>
# include <atomic>
# include <vector>
# include <queue>
# include <functional>
# include <memory>

using Callback = std::function<void()>;

class WorkerPool {
public:
    WorkerPool(size_t poolSize);
    ~WorkerPool();

    class IJob {
    public:
        virtual ~IJob() = default;
        virtual void execute() = 0;
    };

    void addJob(const Callback& jobToExecute);
    void addJob(const std::shared_ptr<IJob>& jobToExecute);

private:
    std::atomic<bool>                   running;
    std::vector<std::thread>            threads;
    std::mutex                          mtx;
    std::queue<std::shared_ptr<IJob>>   jobs;
    std::condition_variable             cv;

    void loop();
};

class FunctionJob : public WorkerPool::IJob {
private:
    Callback job;

public:
    FunctionJob(const Callback& jobToExecute);
    void execute() override;
};

#endif