#ifndef LIBFTPP_SERVER_HPP
# define LIBFTPP_SERVER_HPP

# include "network/message.hpp"
# include <functional>
# include <vector>
# include <queue>
# include <atomic>
# include <thread>
# include <mutex>
# include <map>
# include <unordered_map>
# include <stdexcept>
# include <string>

class Server {
public:
    using ClientID = long long;
    using Action = std::function<void(ClientID& clientID, const Message& msg)>; // Mantener const Message&

    Server();
    ~Server();

    void start(const size_t& port);
    void defineAction(const Message::Type& messageType, const Action& action);
    void sendTo(const Message& message, ClientID clientID);
    void sendToArray(const Message& message, std::vector<ClientID> clientIDs);
    void sendToAll(const Message& message);
    void update();

    class AlreadyStartedException : public std::exception {
        const char* what() const noexcept;
    };

    class NotStartedException : public std::exception {
        const char* what() const noexcept;
    };

    class StartFailedException : public std::runtime_error {
    public:
        explicit StartFailedException(const std::string& msg);
    };

    class UnknownClientException : public std::runtime_error {
    public:
        explicit UnknownClientException();
    };

    class SendingFailedException : public std::runtime_error {
    public:
        explicit SendingFailedException();
    };

    class BatchSendingFailedException : public std::runtime_error {
    public:
        explicit BatchSendingFailedException();
    };

private:
    int serverSocket;
    std::atomic<bool> isRunning;
    std::atomic<bool> shouldStop;

    std::thread acceptorThread;
    std::mutex mutex;
    std::map<ClientID, int> clients;
    std::vector<std::pair<ClientID, Message>> receivedMessages;
    std::map<ClientID, std::queue<Message>> messagesToSend;
    std::unordered_map<Message::Type, Action> actions;

    ClientID nextClientID;

    void acceptorLoop();
    void handleClient(ClientID clientID, int clientSocket);
    void sendToClient(const Message& message, ClientID clientID);
};

#endif