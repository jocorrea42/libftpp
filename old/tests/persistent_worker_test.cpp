#include "libftpp.hpp"
#include <iostream>
#include <atomic>

int main() {
    ft::threadSafeCout.setPrefix("[PersistentWorkerTest] ");
    ft::PersistentWorker pw;
    std::atomic<int> counter{0};
    pw.addTask("t1", [&](){ counter++; });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (counter.load() == 0) {
        ft::threadSafeCout << "FAIL: persistent task did not run" << ft::threadSafeCout.endl();
        return 1;
    }
    pw.removeTask("t1");
    ft::threadSafeCout << "PersistentWorker tests passed." << ft::threadSafeCout.endl();
    return 0;
}
