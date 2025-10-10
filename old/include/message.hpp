#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "data_buffer.hpp"
#include <string>

namespace ft {

class Message {
public:
    Message(int type);
    // construct from type and buffer
    Message(int type, const DataBuffer& buf);
    int type() const;

    // Serialize the message to a raw byte vector: [int32_t type][uint64_t payload_size][payload bytes]
    std::vector<char> serialize() const;

    // Deserialize from raw bytes
    static Message deserialize(const std::vector<char>& raw);

    // Generic POD write
    template<typename T>
    Message& operator<<(const T& value) {
        _buffer << value;
        return *this;
    }

    // string overload
    Message& operator<<(const std::string& s) {
        _buffer << s;
        return *this;
    }

    // Generic POD read
    template<typename T>
    Message& operator>>(T& value) {
        _buffer >> value;
        return *this;
    }

    Message& operator>>(std::string& s) {
        _buffer >> s;
        return *this;
    }

private:
    int _type;
    DataBuffer _buffer;
};

} // namespace ft

#endif // MESSAGE_HPP
