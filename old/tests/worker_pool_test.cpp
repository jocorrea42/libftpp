#include "libftpp.hpp"
#include <iostream>
#include <atomic>

int main() {
    ft::threadSafeCout.setPrefix("[WorkerPoolTest] ");
    ft::WorkerPool wp(2);
    std::atomic<int> counter{0};
    wp.addJob([&](){ counter++; ft::threadSafeCout << "Job A" << ft::threadSafeCout.endl(); });
    wp.addJob([&](){ counter++; ft::threadSafeCout << "Job B" << ft::threadSafeCout.endl(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (counter.load() != 2) {
        ft::threadSafeCout << "FAIL: expected 2 jobs executed, got " << counter.load() << ft::threadSafeCout.endl();
        return 1;
    }
    ft::threadSafeCout << "WorkerPool tests passed." << ft::threadSafeCout.endl();
    return 0;
}
