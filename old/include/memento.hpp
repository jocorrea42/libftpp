#ifndef MEMENTO_HPP
#define MEMENTO_HPP


#include <vector>


namespace ft {


class Memento {
public:
	virtual ~Memento() {}
class Snapshot {
public:
	Snapshot();
	explicit Snapshot(const class DataBuffer& buf);
	class DataBuffer toDataBuffer() const;

	std::vector<char> data;
};


virtual Snapshot save() const;
virtual void load(const Snapshot& snapshot);


protected:
virtual void _saveToSnapshot(Snapshot& snapshot) const = 0;
virtual void _loadFromSnapshot(const Snapshot& snapshot) = 0;
};


} // namespace ft


#endif // MEMENTO_HPP