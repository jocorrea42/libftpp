#include "libftpp.hpp"
#include <iostream>

int main() {
    ft::threadSafeCout.setPrefix("[MementoTest] ");

    class Counter : public ft::Memento {
    public:
        Counter(int v = 0) : _v(v) {}
        void inc() { ++_v; }
        int value() const { return _v; }

    protected:
        void _saveToSnapshot(Snapshot& snapshot) const override {
            ft::DataBuffer buf;
            buf << _v;
            snapshot = Snapshot(buf);
        }

        void _loadFromSnapshot(const Snapshot& snapshot) override {
            ft::DataBuffer buf = snapshot.toDataBuffer();
            buf >> _v;
        }

    private:
        int _v;
    };

    Counter c(5);
    c.inc();
    ft::threadSafeCout << "Counter before save: " << c.value() << ft::threadSafeCout.endl();
    ft::Memento::Snapshot snap = c.save();
    c.inc(); c.inc();
    ft::threadSafeCout << "Counter after inc: " << c.value() << ft::threadSafeCout.endl();
    c.load(snap);
    ft::threadSafeCout << "Counter after load: " << c.value() << ft::threadSafeCout.endl();

    return 0;
}
