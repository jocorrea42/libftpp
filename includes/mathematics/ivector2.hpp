// includes/mathematics/ivector2.hpp
#ifndef IVECTOR2_HPP
#define IVECTOR2_HPP

#include "data_structures/data_buffer.hpp"
#include "iostreams/thread_safe_iostream.hpp"
#include <cmath>
#include <iostream>

template<typename T>
struct IVector2 {
    T x, y;

    // Constructores
    IVector2() : x(T()), y(T()) {}
    IVector2(T x, T y) : x(x), y(y) {}
    
    // Operadores aritméticos
    IVector2 operator+(const IVector2& other) const {
        return IVector2(x + other.x, y + other.y);
    }
    
    IVector2 operator-(const IVector2& other) const {
        return IVector2(x - other.x, y - other.y);
    }
    
    IVector2 operator*(const IVector2& other) const {
        return IVector2(x * other.x, y * other.y);
    }
    
    IVector2 operator/(const IVector2& other) const {
        return IVector2(x / other.x, y / other.y);
    }
    
    IVector2 operator*(T scalar) const {
        return IVector2(x * scalar, y * scalar);
    }
    
    IVector2 operator/(T scalar) const {
        return IVector2(x / scalar, y / scalar);
    }
    
    // Operadores de comparación
    bool operator==(const IVector2& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const IVector2& other) const {
        return !(*this == other);
    }
    
    // Métodos matemáticos
    float length() const {
        return std::sqrt(x*x + y*y);
    }
    
    IVector2<float> normalize() const {
        float len = length();
        if (len == 0.0f) return IVector2<float>(0, 0);
        return IVector2<float>(x/len, y/len);
    }
    
    float dot(const IVector2& other) const {
        return x * other.x + y * other.y;
    }
    
    IVector2 cross() const {
        return IVector2(y, -x); // Producto cruz 2D
    }
    
    // Serialización con DataBuffer
    void serialize(DataBuffer& buffer) const {
        buffer << x << y;
    }
    
    void deserialize(DataBuffer& buffer) {
        buffer >> x >> y;
    }
    
    // Conversión a string
    std::string toString() const {
        return "IVector2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// Operadores globales para escalares
template<typename T>
IVector2<T> operator*(T scalar, const IVector2<T>& vec) {
    return vec * scalar;
}

// Integración con ThreadSafeIOStream
template<typename T>
ThreadSafeIOStream& operator<<(ThreadSafeIOStream& stream, const IVector2<T>& vec) {
    stream << vec.toString();
    return stream;
}

// Integración con DataBuffer
template<typename T>
DataBuffer& operator<<(DataBuffer& buffer, const IVector2<T>& vec) {
    vec.serialize(buffer);
    return buffer;
}

template<typename T>
DataBuffer& operator>>(DataBuffer& buffer, IVector2<T>& vec) {
    vec.deserialize(buffer);
    return buffer;
}

#endif // IVECTOR2_HPP