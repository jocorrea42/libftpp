#include <iostream>
#include <string>
#include "data_buffer.hpp"

int main() {
    DataBuffer db;
    db << 42 << std::string("hola") << 3.1415;

    int a;
    std::string s;
    double d;

    db.resetReadPos();
    db >> a >> s >> d;

    std::cout << "a=" << a << " s=\"" << s << "\" d=" << d << std::endl;
    return 0;
}
