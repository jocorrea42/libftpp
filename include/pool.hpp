#ifndef POOL_HPP
#define POOL_HPP

#include <vector>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <cstddef>

/*
 * Pool<TType>
 * - resize(n): prepara N slots sin construir los objetos todavía
 * - acquire(args...): construye (placement new) un objeto prealocado y devuelve Pool::Object
 *
 * Pool::Object
 * - administra la devolución automática al pool en su destructor
 * - no se puede copiar (delete copy), sí mover
 *
 * Nota: los templates deben implementarse en el header (regla del subject).
 */

template<typename TType>
class Pool {
public:
    class Object {
        friend class Pool<TType>;
    public:
        Object() : _pool(nullptr), _index(static_cast<size_t>(-1)) {}
        // no copy
        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;
        // move
        Object(Object&& o) noexcept : _pool(o._pool), _index(o._index) {
            o._pool = nullptr;
            o._index = static_cast<size_t>(-1);
        }
        Object& operator=(Object&& o) noexcept {
            if (this != &o) {
                release();
                _pool = o._pool;
                _index = o._index;
                o._pool = nullptr;
                o._index = static_cast<size_t>(-1);
            }
            return *this;
        }
        ~Object() { release(); }

        // acceso al objeto
        TType* operator->() {
            return &_pool->getObjectAt(_index);
        }
        TType& operator*() {
            return _pool->getObjectAt(_index);
        }

        // liberar manualmente (opcional)
        void release() {
            if (_pool) {
                _pool->release(_index);
                _pool = nullptr;
                _index = static_cast<size_t>(-1);
            }
        }

    private:
        Object(Pool<TType>* p, size_t idx) : _pool(p), _index(idx) {}
        Pool<TType>* _pool;
        size_t _index;
    };

    Pool() : _count(0) {}
    ~Pool() { clear(); }

    // Reserva espacio para 'number' objetos (no construidos aún)
    void resize(const size_t& number) {
        clear();
        _count = number;
        _storage.resize(number);
        _in_use.assign(number, false);
        _free_indices.reserve(number);
        // cargar índices libre (LIFO)
        for (size_t i = 0; i < number; ++i) {
            _free_indices.push_back(number - 1 - i);
        }
    }

    // Construye y entrega un objeto pre-alocado (tiene que estar template)
    template<typename ... TArgs>
    Object acquire(TArgs&&... args) {
        if (_free_indices.empty()) {
            throw std::runtime_error("Pool::acquire: no free object available");
        }
        size_t idx = _free_indices.back();
        _free_indices.pop_back();
        void* mem = static_cast<void*>(&_storage[idx]);
        // placement new
        new (mem) TType(std::forward<TArgs>(args)...);
        _in_use[idx] = true;
        return Object(this, idx);
    }

private:
    // libera (llama el destructor, no dealloca memoria)
    void release(size_t idx) {
        if (idx >= _count) return;
        if (!_in_use[idx]) return;
        TType* obj = reinterpret_cast<TType*>(&_storage[idx]);
        obj->~TType();
        _in_use[idx] = false;
        _free_indices.push_back(idx);
    }

    TType& getObjectAt(size_t idx) {
        if (idx >= _count || !_in_use[idx]) {
            throw std::runtime_error("Pool::getObjectAt: invalid access");
        }
        return *reinterpret_cast<TType*>(&_storage[idx]);
    }

    void clear() {
        for (size_t i = 0; i < _storage.size(); ++i) {
            if (_in_use[i]) {
                TType* obj = reinterpret_cast<TType*>(&_storage[i]);
                obj->~TType();
            }
        }
        _storage.clear();
        _in_use.clear();
        _free_indices.clear();
        _count = 0;
    }

    // almacenamiento crudo alineado
    std::vector<typename std::aligned_storage<sizeof(TType), alignof(TType)>::type> _storage;
    std::vector<bool> _in_use;
    std::vector<size_t> _free_indices;
    size_t _count;
};

#endif // POOL_HPP
