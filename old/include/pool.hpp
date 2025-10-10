#ifndef POOL_HPP
#define POOL_HPP

#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_set>

namespace ft {

template <typename TType>
class Pool {
public:
    class Object {
    public:
        Object(TType* obj, Pool<TType>& pool)
            : _obj(obj), _pool(pool), _active(true) {}

        // non-copyable
        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;

        // movable
        Object(Object&& other) noexcept
            : _obj(other._obj), _pool(other._pool), _active(other._active) {
            other._obj = nullptr;
            other._active = false;
        }

        Object& operator=(Object&& other) noexcept {
            if (this != &other) {
                release();
                _obj = other._obj;
                _active = other._active;
                // keep _pool reference
                other._obj = nullptr;
                other._active = false;
            }
            return *this;
        }

        ~Object() { release(); }

        TType* operator->() { return _obj; }

        TType& operator*() { return *_obj; }

        void release() {
            if (_active && _obj) {
                // call destructor
                _obj->~TType();
                // mark as not constructed in pool and return to free list
                _pool._constructed.erase(_obj);
                _pool._objects.push_back(_obj);
                _active = false;
            }
        }

    private:
        TType* _obj;
        Pool<TType>& _pool;
        bool _active;
    };

    void resize(const size_t& numberOfObjects) {
        _objects.reserve(numberOfObjects);
        for (size_t i = 0; i < numberOfObjects; ++i) {
            // allocate raw memory without constructing TType
            TType* mem = static_cast<TType*>(::operator new(sizeof(TType)));
            _rawObjects.push_back(mem);
            _objects.push_back(mem);
        }
    }

    template <typename... TArgs>
    Object acquire(TArgs&&... args) {
        if (_objects.empty())
            throw std::runtime_error("No objects available in pool");

        TType* obj = _objects.back();
        _objects.pop_back();
        new (obj) TType(std::forward<TArgs>(args)...); // placement new
        _constructed.insert(obj);
        return Object(obj, *this);
    }

private:
    std::vector<TType*> _objects; // free list
    std::vector<TType*> _rawObjects; // all allocated raw memory
    std::unordered_set<TType*> _constructed; // pointers currently constructed
    
public:
    ~Pool() {
        // destroy any constructed objects and free raw memory
        for (TType* ptr : _rawObjects) {
            if (_constructed.find(ptr) != _constructed.end()) {
                ptr->~TType();
            }
            ::operator delete(ptr);
        }
    }
};

} // namespace ft

#endif
