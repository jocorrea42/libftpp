#include "network/server.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <cstring>
#include <errno.h>
#include <iostream>

Server::Server()
: serverSocket(-1), isRunning(false), shouldStop(false), nextClientID(1) {}

Server::~Server() {
    shouldStop = true;
    isRunning = false;
    
    if (acceptorThread.joinable()) {
        acceptorThread.join();
    }
    
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& client : clients) {
        close(client.second);
    }
    if (serverSocket != -1) {
        close(serverSocket);
    }
}

void Server::start(const size_t& port) {
    if (isRunning) {
        throw AlreadyStartedException();
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw StartFailedException("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(serverSocket);
        throw StartFailedException("Failed to set socket options");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(serverSocket);
        throw StartFailedException("Failed to bind socket");
    }

    if (listen(serverSocket, 10) < 0) {
        close(serverSocket);
        throw StartFailedException("Failed to listen on socket");
    }

    isRunning = true;
    acceptorThread = std::thread(&Server::acceptorLoop, this);
}

void Server::defineAction(const Message::Type& messageType, const Action& action) {
    std::lock_guard<std::mutex> lock(mutex);
    actions[messageType] = action;
}

void Server::sendTo(const Message& message, ClientID clientID) {
    if (!isRunning) {
        throw NotStartedException();
    }

    std::lock_guard<std::mutex> lock(mutex);
    if (clients.find(clientID) == clients.end()) {
        throw UnknownClientException();
    }
    messagesToSend[clientID].push(message);
}

void Server::sendToArray(const Message& message, std::vector<ClientID> clientIDs) {
    if (!isRunning) {
        throw NotStartedException();
    }

    bool error = false;
    for (ClientID clientID : clientIDs) {
        try {
            sendTo(message, clientID);
        } catch (const UnknownClientException&) {
            error = true;
        }
    }

    if (error) {
        throw BatchSendingFailedException();
    }
}

void Server::sendToAll(const Message& message) {
    if (!isRunning) {
        throw NotStartedException();
    }

    std::lock_guard<std::mutex> lock(mutex);
    bool error = false;
    
    for (auto& client : clients) {
        try {
            messagesToSend[client.first].push(message);
        } catch (...) {
            error = true;
        }
    }

    if (error) {
        throw BatchSendingFailedException();
    }
}

void Server::update() {
    if (!isRunning) {
        throw NotStartedException();
    }

    std::vector<std::pair<ClientID, Message>> messagesToProcess;
    std::unordered_map<Message::Type, Action> currentActions;

    {
        std::lock_guard<std::mutex> lock(mutex);
        messagesToProcess.swap(receivedMessages);
        currentActions = actions;
    }

    for (auto& pair : messagesToProcess) {
        ClientID clientID = pair.first;
        Message& msg = pair.second;
        
        auto it = currentActions.find(msg.type());
        if (it != currentActions.end() && it->second) {
            it->second(clientID, msg);
        }
    }
}

void Server::acceptorLoop() {
    while (isRunning && !shouldStop) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(serverSocket + 1, &readfds, NULL, NULL, &timeout);
        
        if (activity > 0 && FD_ISSET(serverSocket, &readfds)) {
            sockaddr_in clientAddr{};
            socklen_t clientLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            
            if (clientSocket >= 0) {
                ClientID clientID = nextClientID++;
                std::lock_guard<std::mutex> lock(mutex);
                clients[clientID] = clientSocket;
                messagesToSend[clientID] = std::queue<Message>();
                std::thread(&Server::handleClient, this, clientID, clientSocket).detach();
            }
        }

        // Handle sending pending messages
        std::lock_guard<std::mutex> lock(mutex);
        for (auto it = messagesToSend.begin(); it != messagesToSend.end(); ) {
            ClientID clientID = it->first;
            if (clients.find(clientID) == clients.end()) {
                it = messagesToSend.erase(it);
                continue;
            }

            while (!it->second.empty()) {
                try {
                    sendToClient(it->second.front(), clientID);
                    it->second.pop();
                } catch (const SendingFailedException&) {
                    // Remove disconnected client
                    close(clients[clientID]);
                    clients.erase(clientID);
                    messagesToSend.erase(it);
                    break;
                }
            }
            ++it;
        }
    }
}

void Server::handleClient(ClientID clientID, int clientSocket) {
    while (isRunning && !shouldStop) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(clientSocket, &readfds);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; // 100ms

        int activity = select(clientSocket + 1, &readfds, NULL, NULL, &timeout);
        
        if (activity > 0 && FD_ISSET(clientSocket, &readfds)) {
            // Read message size
            size_t messageSize;
            ssize_t bytesRead = ::recv(clientSocket, &messageSize, sizeof(messageSize), MSG_WAITALL);
            
            if (bytesRead <= 0) {
                break; // Client disconnected
            }

            if (bytesRead == sizeof(messageSize)) {
                // Read message data
                std::vector<char> messageData(messageSize);
                bytesRead = ::recv(clientSocket, messageData.data(), messageSize, MSG_WAITALL);
                
                if (bytesRead <= 0 || static_cast<size_t>(bytesRead) != messageSize) {
                    break; // Client disconnected
                }

                // Deserialize and queue the message
                Message msg(0);
                try {
                    std::string dataStr(messageData.data(), messageSize);
                    msg.deserialize(dataStr);
                    
                    std::lock_guard<std::mutex> lock(mutex);
                    receivedMessages.push_back(std::make_pair(clientID, msg));
                } catch (const std::exception&) {
                    // Ignore malformed messages
                }
            }
        }
    }

    // Clean up client
    std::lock_guard<std::mutex> lock(mutex);
    close(clientSocket);
    clients.erase(clientID);
    messagesToSend.erase(clientID);
}

void Server::sendToClient(const Message& message, ClientID clientID) {
    auto it = clients.find(clientID);
    if (it == clients.end()) {
        throw UnknownClientException();
    }

    std::string serialized = message.serialize();
    size_t messageSize = serialized.size();

    // Send message size first
    ssize_t sent = ::send(it->second, &messageSize, sizeof(messageSize), 0);
    if (sent != sizeof(messageSize)) {
        throw SendingFailedException();
    }

    // Send message data
    sent = ::send(it->second, serialized.data(), messageSize, 0);
    if (sent != static_cast<ssize_t>(messageSize)) {
        throw SendingFailedException();
    }
}

const char* Server::AlreadyStartedException::what() const noexcept {
    return "Server: Already started.";
}

const char* Server::NotStartedException::what() const noexcept {
    return "Server: Not started.";
}

Server::StartFailedException::StartFailedException(const std::string& msg)
: std::runtime_error("Server: " + msg + ".") {}

Server::UnknownClientException::UnknownClientException()
: std::runtime_error("Server: Unknown client.") {}

Server::SendingFailedException::SendingFailedException()
: std::runtime_error("Server: Failed to send message.") {}

Server::BatchSendingFailedException::BatchSendingFailedException()
: std::runtime_error("Server: Failed to send at least 1 message.") {}