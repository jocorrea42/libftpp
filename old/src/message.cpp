#include "../include/message.hpp"

namespace ft {

Message::Message(int type) : _type(type) {}

int Message::type() const { return _type; }

Message::Message(int type, const DataBuffer& buf)
	: _type(type), _buffer(buf) {}

std::vector<char> Message::serialize() const {
	DataBuffer header;
	int32_t t = static_cast<int32_t>(_type);
	uint64_t payload_size = _buffer.size();
	header << t;
	header << payload_size;
	std::vector<char> out;
	// header raw
	const auto& hr = header.raw();
	out.insert(out.end(), hr.begin(), hr.end());
	// payload
	const auto& pr = _buffer.raw();
	out.insert(out.end(), pr.begin(), pr.end());
	return out;
}

Message Message::deserialize(const std::vector<char>& raw) {
	DataBuffer buf;
	buf.fromRaw(raw);
	int32_t t;
	uint64_t payload_size;
	buf >> t;
	buf >> payload_size;
	// read remaining payload_size bytes into DataBuffer
	DataBuffer payload;
	std::vector<char> payload_data;
	size_t pos = sizeof(int32_t) + sizeof(uint64_t);
	if (raw.size() < pos + payload_size) throw std::runtime_error("Invalid raw message");
	payload_data.insert(payload_data.end(), raw.begin() + pos, raw.begin() + pos + payload_size);
	payload.fromRaw(payload_data);
	return Message(static_cast<int>(t), payload);
}

} // namespace ft
