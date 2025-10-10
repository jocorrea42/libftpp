#ifndef THREAD_HPP
#define THREAD_HPP


#include <thread>
#include <functional>
#include <string>


namespace ft {


class Thread {
public:
Thread(const std::string& name, const std::function<void()>& functToExecute)
: _name(name), _function(functToExecute) {}


void start() { _thread = std::thread(_function); }
void stop() { if (_thread.joinable()) _thread.join(); }


std::string getName() const { return _name; }


private:
std::string _name;
std::function<void()> _function;
std::thread _thread;
};


} // namespace ft


#endif // THREAD_HPP