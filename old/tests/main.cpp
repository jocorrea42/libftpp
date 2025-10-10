#include "libftpp.hpp"
#include <iostream>
#include <thread>

int main() {
    // ================= ThreadSafeIOStream =================
    ft::threadSafeCout.setPrefix("[Main] ");
    ft::threadSafeCout << "Starting libftpp tests..." << ft::threadSafeCout.endl();

    // ================= Pool =================
    ft::Pool<int> pool;
    pool.resize(5);
    {
        auto obj = pool.acquire();
        *obj = 42;
        ft::threadSafeCout << "Pool object value: " << *obj << ft::threadSafeCout.endl();
    } // obj liberado automáticamente

    // ================= ThreadSafeQueue =================
    ft::ThreadSafeQueue<int> queue;
    queue.push_back(10);
    queue.push_front(5);
    ft::threadSafeCout << "Queue pop front: " << queue.pop_front() << ft::threadSafeCout.endl();
    ft::threadSafeCout << "Queue pop back: " << queue.pop_back() << ft::threadSafeCout.endl();

    // ================= WorkerPool =================
    ft::WorkerPool poolThreads(2);
    poolThreads.addJob([](){ ft::threadSafeCout << "Job 1 running" << ft::threadSafeCout.endl(); });
    poolThreads.addJob([](){ ft::threadSafeCout << "Job 2 running" << ft::threadSafeCout.endl(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ================= PersistentWorker =================
    ft::PersistentWorker worker;
    worker.addTask("task1", [](){ ft::threadSafeCout << "Persistent task executing" << ft::threadSafeCout.endl(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    worker.removeTask("task1");
    ft::threadSafeCout << "All tests completed." << ft::threadSafeCout.endl();
    return 0;
}
