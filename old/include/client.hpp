#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "message.hpp"
#include "thread_safe_queue.hpp"
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <atomic>

namespace ft {

class Client {
public:
    Client();
    ~Client();

    void connect(const std::string& address, const size_t& port);
    void disconnect();

    void send(const Message& message);

    void defineAction(const int& messageType, const std::function<void(const Message&)>& action);
    void update();

private:
    int _sockfd{-1};
    std::thread _readerThread;
    std::atomic<bool> _running{false};
    ft::ThreadSafeQueue<Message> _incoming;
    std::map<int, std::function<void(const Message&)>> _actions;

    void readerLoop();
};

} // namespace ft

#endif // CLIENT_HPP
