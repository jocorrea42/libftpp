#ifndef PERSISTENT_WORKER_HPP
#define PERSISTENT_WORKER_HPP


#include <string>
#include <functional>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>


namespace ft {


class PersistentWorker {
public:
PersistentWorker();
~PersistentWorker();


void addTask(const std::string& name, const std::function<void()>& jobToExecute);
void removeTask(const std::string& name);


private:
std::map<std::string, std::function<void()>> _tasks;
std::mutex _mutex;
std::thread _thread;
std::atomic<bool> _running{true};
std::condition_variable _cv;


void run();
};


} // namespace ft


#endif // PERSISTENT_WORKER_HPP