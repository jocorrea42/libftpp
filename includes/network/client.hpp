#ifndef LIBFTPP_CLIENT_HPP
# define LIBFTPP_CLIENT_HPP

# include "network/message.hpp"
# include <functional>
# include <queue>
# include <atomic>
# include <thread>
# include <mutex>
# include <unordered_map>
# include <stdexcept>
# include <string>

class Client {
public:
    using Action = std::function<void(const Message& msg)>;

    Client();
    ~Client();

    void connect(const std::string& address, const size_t& port);
    void disconnect();
    void defineAction(const Message::Type& messageType, const Action& action);
    void send(const Message& message);
    void update();

    class AlreadyConnectedException : public std::exception {
        const char* what() const noexcept;
    };

    class NotConnectedException : public std::exception {
        const char* what() const noexcept;
    };

    class ConnectionFailedException : public std::runtime_error {
    public:
        explicit ConnectionFailedException(const std::string& msg);
    };

    class SendingFailedException : public std::runtime_error {
    public:
        explicit SendingFailedException();
    };

private:
    int sockfd;
    std::atomic<bool> isConnected;
    std::atomic<bool> shouldStop;

    std::thread receiverThread;
    std::mutex mutex;
    std::queue<Message> receivedMessages;
    std::queue<Message> messagesToSend;
    std::unordered_map<Message::Type, Action> actions;

    void receiverLoop();
    void sendMessage(const Message& message);
};

#endif