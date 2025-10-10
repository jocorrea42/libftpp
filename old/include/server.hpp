#ifndef SERVER_HPP
#define SERVER_HPP

#include "message.hpp"
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <atomic>

namespace ft {

class Server {
public:
    Server();
    ~Server();

    void start(const size_t& port);
    void stop();

    void defineAction(const int& messageType, const std::function<void(long long&, const Message&)>& action);

private:
    int _sockfd{-1};
    std::thread _acceptThread;
    std::atomic<bool> _running{false};
    std::map<int, std::function<void(long long&, const Message&)>> _actions;

    void acceptLoop();
    void clientHandler(int clientSock, long long clientId);
};

} // namespace ft

#endif // SERVER_HPP
