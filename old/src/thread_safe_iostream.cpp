#include "../include/thread_safe_iostream.hpp"
#include <iostream>

namespace ft {

ThreadSafeIOStream::ThreadSafeIOStream() : _prefix("") {}

void ThreadSafeIOStream::setPrefix(const std::string& prefix) { _prefix = prefix; }

ThreadSafeIOStream& ThreadSafeIOStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
	std::lock_guard<std::mutex> lock(_mutex);
	manip(_buffer);
	return *this;
}

void ThreadSafeIOStream::pushNewline() {
	std::lock_guard<std::mutex> lock(_mutex);
	_buffer << std::endl;
}

void ThreadSafeIOStream::flush() {
	std::lock_guard<std::mutex> lock(_mutex);
	std::cout << _buffer.str();
	std::cout.flush();
	// clear the buffer
	_buffer.str(std::string());
	_buffer.clear();
}

// define thread-local instance
thread_local ThreadSafeIOStream threadSafeCout;

ThreadSafeEndl ThreadSafeIOStream::endl() { return ThreadSafeEndl(); }

ThreadSafeIOStream& operator<<(ThreadSafeIOStream& stream, ThreadSafeEndl manip) {
	(void)manip;
	// push newline then flush
	stream.pushNewline();
	stream.flush();
	return stream;
}

} // namespace ft