#include "../include/client.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <iostream>

namespace ft {

Client::Client() {}

Client::~Client() { disconnect(); }

void Client::connect(const std::string& address, const size_t& port) {
    struct sockaddr_in serv_addr;
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr);
    ::connect(_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    _running = true;
    std::cout << "[Client] starting reader thread" << std::endl;
    _readerThread = std::thread([this]() { readerLoop(); });
}

void Client::disconnect() {
    _running = false;
    if (_sockfd != -1) { close(_sockfd); _sockfd = -1; }
    if (_readerThread.joinable()) _readerThread.join();
}

void Client::send(const Message& message) {
    auto raw = message.serialize();
    // send all
    size_t total = 0;
    while (total < raw.size()) {
        ssize_t sent = ::send(_sockfd, raw.data() + total, raw.size() - total, 0);
        if (sent <= 0) break;
        total += sent;
    }
    std::cout << "[Client] send complete, bytes=" << total << std::endl;
}

void Client::defineAction(const int& messageType, const std::function<void(const Message&)>& action) {
    _actions[messageType] = action;
}

void Client::readerLoop() {
    std::cout << "[Client] readerLoop start" << std::endl;
    while (_running) {
        // read header (int32 + uint64)
        char header[12];
        ssize_t r = ::recv(_sockfd, header, 12, MSG_WAITALL);
        if (r <= 0) break;
        std::vector<char> raw(header, header+12);
        DataBuffer db;
        db.fromRaw(raw);
        int32_t t; uint64_t sz;
        db >> t; db >> sz;
        std::vector<char> payload(sz);
        ssize_t pr = ::recv(_sockfd, payload.data(), sz, MSG_WAITALL);
        if (pr <= 0) break;
        raw.insert(raw.end(), payload.begin(), payload.end());
        try {
            Message m = Message::deserialize(raw);
            _incoming.push_back(m);
        } catch (...) {}
    }
    std::cout << "[Client] readerLoop exiting" << std::endl;
}

void Client::update() {
    try {
        Message m = _incoming.pop_front();
        auto it = _actions.find(m.type());
        if (it != _actions.end()) it->second(m);
    } catch (...) {}
}

} // namespace ft
