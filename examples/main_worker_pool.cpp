#include "threading.hpp"
#include "iostreams/thread_safe_iostream.hpp"

#include <iostream>

int main() {
    WorkerPool pool(4);

    auto job = []() {
        threadSafeCout << "Executing job on thread: " << std::this_thread::get_id() << std::endl;
    };

    for (int i = 0; i < 1000; ++i) {
        pool.addJob(job);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for jobs to finish

    return 0;
}
