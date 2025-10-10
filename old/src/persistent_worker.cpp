// =================== persistent_worker.cpp ===================
#include "persistent_worker.hpp"


namespace ft {


PersistentWorker::PersistentWorker() {
_thread = std::thread([this]() { run(); });
}


PersistentWorker::~PersistentWorker() {
_running = false;
_cv.notify_all();
if (_thread.joinable()) _thread.join();
}


void PersistentWorker::addTask(const std::string& name, const std::function<void()>& jobToExecute) {
std::lock_guard<std::mutex> lock(_mutex);
_tasks[name] = jobToExecute;
_cv.notify_all();
}


void PersistentWorker::removeTask(const std::string& name) {
std::lock_guard<std::mutex> lock(_mutex);
_tasks.erase(name);
}


void PersistentWorker::run() {
while (_running) {
std::map<std::string, std::function<void()>> tasksCopy;
{
std::unique_lock<std::mutex> lock(_mutex);
tasksCopy = _tasks;
}
for (auto it = tasksCopy.begin(); it != tasksCopy.end(); ++it) {
    it->second();
}
std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
}


} // namespace ft