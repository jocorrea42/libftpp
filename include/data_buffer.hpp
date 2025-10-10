#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <stdexcept>
#include <cstring>

/*
 * DataBuffer
 * - Guarda bloques de bytes con prefijo de longitud (uint32_t)
 * - operator<< y operator>> están implementados como templates en el header
 * - Métodos no-template van en data_buffer.cpp
 */

class DataBuffer {
public:
    DataBuffer();
    ~DataBuffer();

    void clear();
    void resetReadPos();

    size_t size() const;
    const std::vector<char>& raw() const;

    // Serialización: convierte obj a string con ostringstream, guarda [len][bytes]
    template<typename T>
    DataBuffer& operator<<(const T& obj) {
        std::ostringstream oss;
        oss << obj;
        std::string s = oss.str();
        uint32_t len = static_cast<uint32_t>(s.size());
        char lenbuf[sizeof(uint32_t)];
        std::memcpy(lenbuf, &len, sizeof(uint32_t));
        _data.insert(_data.end(), lenbuf, lenbuf + sizeof(uint32_t));
        _data.insert(_data.end(), s.begin(), s.end());
        return *this;
    }

    // Deserialización: lee [len][bytes] y usa istringstream >> out
    template<typename T>
    DataBuffer& operator>>(T& out) {
        if (_readPos + sizeof(uint32_t) > _data.size()) {
            throw std::runtime_error("DataBuffer::operator>>: insufficient data for length");
        }
        uint32_t len = 0;
        std::memcpy(&len, &_data[_readPos], sizeof(uint32_t));
        _readPos += sizeof(uint32_t);
        if (_readPos + len > _data.size()) {
            throw std::runtime_error("DataBuffer::operator>>: insufficient data for object");
        }
        std::string s(&_data[_readPos], &_data[_readPos + len]);
        _readPos += len;
        std::istringstream iss(s);
        iss >> out;
        return *this;
    }

private:
    std::vector<char> _data;
    size_t _readPos;
};

#endif // DATA_BUFFER_HPP
