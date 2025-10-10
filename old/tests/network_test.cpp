#include <libftpp.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>

int main() {
    ft::threadSafeCout.setPrefix("[NetworkTest] ");
    ft::Server server;
    server.start(12345);

    std::atomic<int> received{0};
    server.defineAction(1, [&](long long& clientId, const ft::Message& m){
        (void)clientId;
        int a; std::string s;
        ft::Message copy = m;
        copy >> a >> s;
        received += a;
        std::cout << "Server action invoked, got: " << a << " '" << s << "'" << std::endl;
    });

    ft::Client client;
    client.connect("127.0.0.1", 12345);
    ft::Message msg(1);
    msg << 7 << std::string("net");
    client.send(msg);
    std::cout << "Client sent message" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (received.load() != 7) {
        ft::threadSafeCout << "FAIL: expected 7, got " << received.load() << ft::threadSafeCout.endl();
        return 1;
    }

    std::cout << "Client disconnecting..." << std::endl;
    client.disconnect();
    std::cout << "Client disconnected." << std::endl;
    std::cout << "Server stopping..." << std::endl;
    server.stop();
    std::cout << "Server stopped." << std::endl;
    ft::threadSafeCout << "Network tests passed." << ft::threadSafeCout.endl();
    return 0;
}
