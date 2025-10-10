#include "libftpp.hpp"
#include <iostream>

int main() {
    ft::threadSafeCout.setPrefix("[MessageTest] ");
    ft::Message msg(1);
    msg << 42 << std::string("hello");

    auto raw = msg.serialize();
    auto msg2 = ft::Message::deserialize(raw);
    int a; std::string s;
    msg2 >> a >> s;
    if (a != 42 || s != "hello") {
        ft::threadSafeCout << "FAIL: message serialization mismatch" << ft::threadSafeCout.endl();
        return 1;
    }
    ft::threadSafeCout << "Message tests passed (serialize/deserialize)." << ft::threadSafeCout.endl();
    return 0;
}
