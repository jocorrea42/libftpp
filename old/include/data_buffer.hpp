#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP


#include <vector>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <cstdint>
#include <type_traits>


namespace ft {


class DataBuffer {
public:
	DataBuffer() : _readPos(0) {}

	// Generic POD write (appends raw bytes)
	template<typename T>
	DataBuffer& operator<<(const T& data) {
		static_assert(std::is_trivially_copyable<T>::value, "DataBuffer only supports trivially copyable types via this operator");
		const char* ptr = reinterpret_cast<const char*>(&data);
		_buffer.insert(_buffer.end(), ptr, ptr + sizeof(T));
		return *this;
	}

	// Generic POD read (reads from current read position)
	template<typename T>
	DataBuffer& operator>>(T& data) {
		static_assert(std::is_trivially_copyable<T>::value, "DataBuffer only supports trivially copyable types via this operator");
		if (_readPos + sizeof(T) > _buffer.size())
			throw std::runtime_error("DataBuffer: not enough data to read");
		std::memcpy(reinterpret_cast<char*>(&data), _buffer.data() + _readPos, sizeof(T));
		_readPos += sizeof(T);
		return *this;
	}

	// Specialization-like overload for std::string
	DataBuffer& operator<<(const std::string& s) {
		uint64_t len = s.size();
		*this << len;
		_buffer.insert(_buffer.end(), s.begin(), s.end());
		return *this;
	}

	DataBuffer& operator>>(std::string& s) {
		uint64_t len;
		*this >> len;
		if (_readPos + len > _buffer.size())
			throw std::runtime_error("DataBuffer: not enough data to read string");
		s.assign(_buffer.data() + _readPos, _buffer.data() + _readPos + len);
		_readPos += len;
		return *this;
	}

	void clear() { _buffer.clear(); _readPos = 0; }
	size_t size() const { return _buffer.size(); }
	void resetReadPos() { _readPos = 0; }
	const std::vector<char>& raw() const { return _buffer; }
	void fromRaw(const std::vector<char>& raw) { _buffer = raw; _readPos = 0; }


private:
	std::vector<char> _buffer;
	size_t _readPos;
};


} // namespace ft


#endif // DATA_BUFFER_HPP