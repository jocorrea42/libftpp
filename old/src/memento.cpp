// =================== memento.cpp ===================
#include "memento.hpp"
#include "data_buffer.hpp"

using namespace ft;


namespace ft {


Memento::Snapshot Memento::save() const {
Snapshot snap;
_saveToSnapshot(snap);
return snap;
}


void Memento::load(const Snapshot& snapshot) {
_loadFromSnapshot(snapshot);
}

Memento::Snapshot::Snapshot() {}

Memento::Snapshot::Snapshot(const DataBuffer& buf) {
	data = buf.raw();
}

DataBuffer Memento::Snapshot::toDataBuffer() const {
	DataBuffer buf;
	buf.fromRaw(data);
	return buf;
}


} // namespace ft