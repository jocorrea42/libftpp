#include "libftpp.hpp"
#include <iostream>

class MySingle {
public:
    MySingle(int v): val(v) {}
    int val;
};

int main() {
    ft::threadSafeCout.setPrefix("[SingletonTest] ");
    // instantiate
    ft::Singleton<MySingle>::instantiate(42);
    MySingle* inst = ft::Singleton<MySingle>::instance();
    if (!inst || inst->val != 42) {
        ft::threadSafeCout << "FAIL: instance value mismatch" << ft::threadSafeCout.endl();
        return 1;
    }

    // attempting to re-instantiate should throw
    bool threw = false;
    try {
        ft::Singleton<MySingle>::instantiate(7);
    } catch (...) {
        threw = true;
    }
    if (!threw) {
        ft::threadSafeCout << "FAIL: re-instantiate did not throw" << ft::threadSafeCout.endl();
        return 1;
    }

    ft::threadSafeCout << "Singleton tests passed." << ft::threadSafeCout.endl();
    return 0;
}
