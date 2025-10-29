#ifndef LIBFTPP_MESSAGE_HPP
# define LIBFTPP_MESSAGE_HPP

# include <string>
# include <vector>
# include <stdexcept>
# include <cstring>

class Message {
public:
    using Type = int;

    explicit Message(Type type = 0);
    Message(const Message& other);
    Message& operator=(const Message& other);
    
    Type type() const;

    template <typename T>
    Message& operator<<(const T& data);
    
    template <typename T>
    Message& operator>>(T& data);

    template <typename T>
    const Message& operator>>(T& data) const;

    std::string serialize() const;
    void deserialize(const std::string& data);

    void resetRead() const;

    class DeserializationFailedException : public std::runtime_error {
    public:
        explicit DeserializationFailedException(const std::string& msg);
    };

private:
    Type msgType;
    std::vector<char> buffer;
    mutable size_t readPos;
};

// Declaraciones externas para las especializaciones de std::string
template <>
Message& Message::operator<<(const std::string& data);

template <>
Message& Message::operator>>(std::string& data);

template <>
const Message& Message::operator>>(std::string& data) const;

# include "message.tpp"

#endif