#ifndef THREAD_SAFE_IOSTREAM_HPP
#define THREAD_SAFE_IOSTREAM_HPP

#include <sstream>
#include <string>
#include <mutex>
#include <iostream>

namespace ft {

// Endl manipulator type
struct ThreadSafeEndl {};

class ThreadSafeIOStream {
public:
    ThreadSafeIOStream();

    void setPrefix(const std::string& prefix);

    // Operador para cualquier tipo imprimible
    template <typename T>
    ThreadSafeIOStream& operator<<(const T& value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _buffer << _prefix << value;
        return *this;
    }

    // Operador para manipuladores (std::endl, std::flush, etc.)
    ThreadSafeIOStream& operator<<(std::ostream& (*manip)(std::ostream&));

    // Note: manipulators specific to ThreadSafeIOStream (like ft::endl)
    // will be implemented in the .cpp file. Keep header minimal.

    // Retornar buffer como string
    std::string str() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _buffer.str();
    }

    // push newline into buffer (used by the endl manipulator)
    void pushNewline();

    // flush buffer to stdout (with prefix already included in buffer)
    void flush();

    // member manipulator returning an object usable with operator<<
    ThreadSafeEndl endl(); // member manipulator: returns a ThreadSafeEndl object

private:
    std::ostringstream _buffer;
    std::string _prefix;
    mutable std::mutex _mutex;
};


// Declaration of thread-local global (definition in source file)
extern thread_local ThreadSafeIOStream threadSafeCout;

// operator<< to handle the endl manipulator; defined in .cpp
ThreadSafeIOStream& operator<<(ThreadSafeIOStream& stream, ThreadSafeEndl manip);

} // namespace ft

#endif
