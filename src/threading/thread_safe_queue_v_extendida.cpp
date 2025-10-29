#include "threading/thread_safe_queue.hpp"

// Instanciaciones explícitas para tipos comunes
template class ThreadSafeQueue<int>;
template class ThreadSafeQueue<std::string>;