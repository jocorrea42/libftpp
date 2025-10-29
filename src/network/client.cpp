#include "network/client.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netdb.h>  // Para getaddrinfo
#include <cstring>
#include <errno.h>

Client::Client()
: sockfd(-1), isConnected(false), shouldStop(false) {}

Client::~Client() {
    disconnect();
}

void Client::connect(const std::string& address, const size_t& port) {
    if (isConnected) {
        throw AlreadyConnectedException();
    }

    // Usar getaddrinfo para resolver la dirección (funciona con localhost y IPs)
    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    char portStr[10];
    snprintf(portStr, sizeof(portStr), "%zu", port);

    int ret = getaddrinfo(address.c_str(), portStr, &hints, &result);
    if (ret != 0) {
        throw ConnectionFailedException("Failed to resolve address: " + address);
    }

    // Intentar conectarse a cada dirección devuelta
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0) {
            continue;
        }

        if (::connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break; // Conexión exitosa
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(result);

    if (sockfd < 0) {
        throw ConnectionFailedException("Failed to connect to server: " + address + ":" + std::to_string(port));
    }

    isConnected = true;
    shouldStop = false;
    receiverThread = std::thread(&Client::receiverLoop, this);
}

void Client::disconnect() {
    if (!isConnected) return;

    shouldStop = true;
    isConnected = false;
    
    if (receiverThread.joinable()) {
        receiverThread.join();
    }
    
    close(sockfd);
    sockfd = -1;
    
    std::lock_guard<std::mutex> lock(mutex);
    while (!receivedMessages.empty()) receivedMessages.pop();
    while (!messagesToSend.empty()) messagesToSend.pop();
}

void Client::defineAction(const Message::Type& messageType, const Action& action) {
    std::lock_guard<std::mutex> lock(mutex);
    actions[messageType] = action;
}

void Client::send(const Message& message) {
    if (!isConnected) {
        throw NotConnectedException();
    }

    std::lock_guard<std::mutex> lock(mutex);
    messagesToSend.push(message);
}

void Client::update() {
    if (!isConnected) {
        throw NotConnectedException();
    }

    std::queue<Message> messagesToProcess;
    std::unordered_map<Message::Type, Action> currentActions;

    {
        std::lock_guard<std::mutex> lock(mutex);
        messagesToProcess.swap(receivedMessages);
        currentActions = actions;
    }

    while (!messagesToProcess.empty()) {
        Message msg = std::move(messagesToProcess.front());
        messagesToProcess.pop();

        auto it = currentActions.find(msg.type());
        if (it != currentActions.end() && it->second) {
            it->second(msg);
        }
    }
}

void Client::receiverLoop() {
    while (isConnected && !shouldStop) {
        // Handle sending
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!messagesToSend.empty()) {
                try {
                    sendMessage(messagesToSend.front());
                    messagesToSend.pop();
                } catch (const SendingFailedException&) {
                    shouldStop = true;
                    break;
                }
            }
        }

        // Handle receiving
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms

        int activity = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity > 0 && FD_ISSET(sockfd, &readfds)) {
            // Read message size first
            size_t messageSize;
            ssize_t bytesRead = ::recv(sockfd, &messageSize, sizeof(messageSize), MSG_WAITALL);
            
            if (bytesRead <= 0) {
                shouldStop = true;
                break;
            }

            if (bytesRead == sizeof(messageSize)) {
                // Read message data
                std::vector<char> messageData(messageSize);
                bytesRead = ::recv(sockfd, messageData.data(), messageSize, MSG_WAITALL);
                
                if (bytesRead <= 0 || static_cast<size_t>(bytesRead) != messageSize) {
                    shouldStop = true;
                    break;
                }

                // Deserialize and queue the message
                Message msg(0);
                try {
                    std::string dataStr(messageData.data(), messageSize);
                    msg.deserialize(dataStr);
                    
                    std::lock_guard<std::mutex> lock(mutex);
                    receivedMessages.push(msg);
                } catch (const std::exception&) {
                    // Ignore malformed messages
                }
            }
        }
    }

    isConnected = false;
}

void Client::sendMessage(const Message& message) {
    std::string serialized = message.serialize();
    size_t messageSize = serialized.size();

    // Send message size first
    ssize_t sent = ::send(sockfd, &messageSize, sizeof(messageSize), 0);
    if (sent != sizeof(messageSize)) {
        throw SendingFailedException();
    }

    // Send message data
    sent = ::send(sockfd, serialized.data(), messageSize, 0);
    if (sent != static_cast<ssize_t>(messageSize)) {
        throw SendingFailedException();
    }
}

const char* Client::AlreadyConnectedException::what() const noexcept {
    return "Client: Already connected.";
}

const char* Client::NotConnectedException::what() const noexcept {
    return "Client: Not connected.";
}

Client::ConnectionFailedException::ConnectionFailedException(const std::string& msg)
: std::runtime_error("Client: " + msg + ".") {}

Client::SendingFailedException::SendingFailedException()
: std::runtime_error("Client: Failed to send message.") {}