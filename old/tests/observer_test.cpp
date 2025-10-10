#include "libftpp.hpp"
#include <iostream>

int main() {
    ft::threadSafeCout.setPrefix("[ObserverTest] ");

    ft::Observer<int> obs;
    int counter = 0;

    obs.subscribe(1, [&](){ counter += 1; });
    obs.subscribe(1, [&](){ counter += 10; });
    obs.subscribe(2, [&](){ counter += 100; });

    // notify event 1 -> should add 11
    obs.notify(1);
    if (counter != 11) {
        ft::threadSafeCout << "FAIL: expected 11, got " << counter << ft::threadSafeCout.endl();
        return 1;
    }

    // notify event 2 -> should add 100
    obs.notify(2);
    if (counter != 111) {
        ft::threadSafeCout << "FAIL: expected 111, got " << counter << ft::threadSafeCout.endl();
        return 1;
    }

    // notify unknown event -> no change
    obs.notify(999);
    if (counter != 111) {
        ft::threadSafeCout << "FAIL: expected 111 after unknown event, got " << counter << ft::threadSafeCout.endl();
        return 1;
    }

    ft::threadSafeCout << "Observer tests passed." << ft::threadSafeCout.endl();
    return 0;
}
