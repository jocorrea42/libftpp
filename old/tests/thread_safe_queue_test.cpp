#include "libftpp.hpp"
#include <iostream>

int main() {
    ft::threadSafeCout.setPrefix("[TSQTest] ");
    ft::ThreadSafeQueue<int> queue;
    bool threw = false;
    try { queue.pop_front(); } catch (...) { threw = true; }
    if (!threw) {
        ft::threadSafeCout << "FAIL: pop_front should throw on empty" << ft::threadSafeCout.endl();
        return 1;
    }

    queue.push_back(10);
    queue.push_front(5);
    if (queue.pop_front() != 5) {
        ft::threadSafeCout << "FAIL: pop_front mismatch" << ft::threadSafeCout.endl();
        return 1;
    }
    if (queue.pop_back() != 10) {
        ft::threadSafeCout << "FAIL: pop_back mismatch" << ft::threadSafeCout.endl();
        return 1;
    }

    ft::threadSafeCout << "ThreadSafeQueue tests passed." << ft::threadSafeCout.endl();
    return 0;
}
