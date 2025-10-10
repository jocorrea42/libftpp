#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP


#include <deque>
#include <mutex>
#include <stdexcept>


namespace ft {


template <typename TType>
class ThreadSafeQueue {
public:
void push_back(const TType& newElement) {
std::lock_guard<std::mutex> lock(_mutex);
_queue.push_back(newElement);
}


void push_front(const TType& newElement) {
std::lock_guard<std::mutex> lock(_mutex);
_queue.push_front(newElement);
}


TType pop_back() {
std::lock_guard<std::mutex> lock(_mutex);
if (_queue.empty()) throw std::runtime_error("Queue empty!");
TType val = _queue.back();
_queue.pop_back();
return val;
}


TType pop_front() {
std::lock_guard<std::mutex> lock(_mutex);
if (_queue.empty()) throw std::runtime_error("Queue empty!");
TType val = _queue.front();
_queue.pop_front();
return val;
}


private:
std::deque<TType> _queue;
std::mutex _mutex;
};


} // namespace ft


#endif // THREAD_SAFE_QUEUE_HPP