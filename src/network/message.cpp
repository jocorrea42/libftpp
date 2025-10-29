#include "network/message.hpp"
#include <cstring>

Message::Message(Type type)
: msgType(type), readPos(0) {}

Message::Message(const Message& other)
: msgType(other.msgType), buffer(other.buffer), readPos(0) {}

Message& Message::operator=(const Message& other) {
    if (this != &other) {
        msgType = other.msgType;
        buffer = other.buffer;
        readPos = 0;
    }
    return *this;
}

Message::Type Message::type() const {
    return msgType;
}

std::string Message::serialize() const {
    std::string result;
    result.resize(sizeof(msgType) + buffer.size());
    
    std::memcpy(&result[0], &msgType, sizeof(msgType));
    if (!buffer.empty()) {
        std::memcpy(&result[sizeof(msgType)], buffer.data(), buffer.size());
    }
    
    return result;
}

void Message::deserialize(const std::string& data) {
    if (data.size() < sizeof(msgType)) {
        throw DeserializationFailedException("Data too short for message type");
    }
    
    std::memcpy(&msgType, &data[0], sizeof(msgType));
    buffer.resize(data.size() - sizeof(msgType));
    if (!buffer.empty()) {
        std::memcpy(buffer.data(), &data[sizeof(msgType)], buffer.size());
    }
    readPos = 0;
}

void Message::resetRead() const {
    readPos = 0;
}

Message::DeserializationFailedException::DeserializationFailedException(const std::string& msg)
: std::runtime_error("Message: " + msg + ".") {}

// Definiciones de las especializaciones para std::string
template <>
Message& Message::operator<<(const std::string& data) {
    size_t size = data.size();
    *this << size;
    size_t oldSize = buffer.size();
    buffer.resize(oldSize + size);
    std::memcpy(&buffer[oldSize], data.data(), size);
    return *this;
}

template <>
Message& Message::operator>>(std::string& data) {
    size_t size;
    *this >> size;
    if (readPos + size > buffer.size()) {
        throw DeserializationFailedException("Not enough data to read string");
    }
    data.assign(&buffer[readPos], size);
    readPos += size;
    return *this;
}

template <>
const Message& Message::operator>>(std::string& data) const {
    size_t size;
    *this >> size;
    if (readPos + size > buffer.size()) {
        throw DeserializationFailedException("Not enough data to read string");
    }
    data.assign(&buffer[readPos], size);
    readPos += size;
    return *this;
}