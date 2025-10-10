#include "data_buffer.hpp"

DataBuffer::DataBuffer() : _data(), _readPos(0) {}
DataBuffer::~DataBuffer() {}

void DataBuffer::clear() {
    _data.clear();
    _readPos = 0;
}

void DataBuffer::resetReadPos() {
    _readPos = 0;
}

size_t DataBuffer::size() const {
    return _data.size();
}

const std::vector<char>& DataBuffer::raw() const {
    return _data;
}
