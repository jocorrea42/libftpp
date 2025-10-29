#ifndef LIBFTPP_MESSAGE_TPP
# define LIBFTPP_MESSAGE_TPP
# include "message.hpp"

template <typename T>
Message& Message::operator<<(const T& data) {
    size_t oldSize = buffer.size();
    buffer.resize(oldSize + sizeof(T));
    std::memcpy(&buffer[oldSize], &data, sizeof(T));
    return *this;
}

template <typename T>
Message& Message::operator>>(T& data) {
    if (readPos + sizeof(T) > buffer.size()) {
        throw DeserializationFailedException("Not enough data to read");
    }
    std::memcpy(&data, &buffer[readPos], sizeof(T));
    readPos += sizeof(T);
    return *this;
}

template <typename T>
const Message& Message::operator>>(T& data) const {
    if (readPos + sizeof(T) > buffer.size()) {
        throw DeserializationFailedException("Not enough data to read");
    }
    std::memcpy(&data, &buffer[readPos], sizeof(T));
    readPos += sizeof(T);
    return *this;
}

#endif