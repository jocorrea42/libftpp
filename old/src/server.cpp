#include "../include/server.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>

namespace ft {

Server::Server() {}

Server::~Server() { stop(); }

void Server::start(const size_t& port) {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(_sockfd, (struct sockaddr*)&address, sizeof(address));
    listen(_sockfd, 5);
    _running = true;
    std::cout << "[Server] starting accept thread" << std::endl;
    _acceptThread = std::thread([this]() { acceptLoop(); });
}

void Server::stop() {
    _running = false;
    if (_sockfd != -1) { close(_sockfd); _sockfd = -1; }
    if (_acceptThread.joinable()) _acceptThread.join();
}

void Server::defineAction(const int& messageType, const std::function<void(long long&, const Message&)>& action) {
    _actions[messageType] = action;
}

void Server::acceptLoop() {
    long long clientId = 0;
    while (_running) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int clientSock = accept(_sockfd, (struct sockaddr*)&client_addr, &addrlen);
        if (clientSock < 0) {
            std::cout << "[Server] accept returned <0" << std::endl;
            break;
        }
        std::cout << "[Server] accepted client " << clientId << " sock=" << clientSock << std::endl;
        std::thread(&Server::clientHandler, this, clientSock, clientId++).detach();
    }
}

void Server::clientHandler(int clientSock, long long clientId) {
    std::cout << "[Server] clientHandler start for id=" << clientId << " sock=" << clientSock << std::endl;
    while (_running) {
        char header[12];
        ssize_t r = ::recv(clientSock, header, 12, MSG_WAITALL);
        if (r <= 0) break;
        std::vector<char> raw(header, header+12);
        DataBuffer db; db.fromRaw(raw);
        int32_t t; uint64_t sz; db >> t; db >> sz;
        std::vector<char> payload(sz);
        ssize_t pr = ::recv(clientSock, payload.data(), sz, MSG_WAITALL);
        if (pr <= 0) break;
        raw.insert(raw.end(), payload.begin(), payload.end());
        try {
            Message m = Message::deserialize(raw);
            auto it = _actions.find(m.type());
            if (it != _actions.end()) it->second(clientId, m);
        } catch (...) {}
    }
    std::cout << "[Server] clientHandler exiting for id=" << clientId << std::endl;
    close(clientSock);
}

} // namespace ft
