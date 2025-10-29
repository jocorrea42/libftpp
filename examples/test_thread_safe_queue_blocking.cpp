#include <iostream>
#include <thread>
#include <vector>
#include "threading.hpp"

// Simple deterministic test: one producer puts numbers 1..N, one consumer waits and collects them
int main() {
    ThreadSafeQueue<int> q;
    const int N = 100;
    std::vector<int> collected;
    collected.reserve(N);

    std::thread producer([&]{
        for (int i = 1; i <= N; ++i) {
            q.push_back(i);
        }
    });

    std::thread consumer([&]{
        for (int i = 1; i <= N; ++i) {
            int v;
            q.wait_pop(v);
            collected.push_back(v);
        }
    });

    producer.join();
    consumer.join();

    // Validate deterministic order
    bool ok = true;
    for (int i = 0; i < N; ++i) {
        if (collected[i] != i+1) { ok = false; break; }
    }

    if (ok) std::cout << "PASS: collected in order" << std::endl;
    else std::cout << "FAIL: out-of-order" << std::endl;

    return ok ? 0 : 1;
}
