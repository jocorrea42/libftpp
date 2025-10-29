// includes/mathematics/ivector3.hpp
#ifndef IVECTOR3_HPP
#define IVECTOR3_HPP

#include "data_structures/data_buffer.hpp"
#include "mathematics/ivector2.hpp"
#include <cmath>
#include <iostream>

template<typename T>
struct IVector3 {
    T x, y, z;

    // Constructores
    IVector3() : x(T()), y(T()), z(T()) {}
    IVector3(T x, T y, T z) : x(x), y(y), z(z) {}
    IVector3(const IVector2<T>& vec2, T z = T()) : x(vec2.x), y(vec2.y), z(z) {}
    
    // Operadores aritméticos
    IVector3 operator+(const IVector3& other) const {
        return IVector3(x + other.x, y + other.y, z + other.z);
    }
    
    IVector3 operator-(const IVector3& other) const {
        return IVector3(x - other.x, y - other.y, z - other.z);
    }
    
    IVector3 operator*(const IVector3& other) const {
        return IVector3(x * other.x, y * other.y, z * other.z);
    }
    
    IVector3 operator/(const IVector3& other) const {
        return IVector3(x / other.x, y / other.y, z / other.z);
    }
    
    IVector3 operator*(T scalar) const {
        return IVector3(x * scalar, y * scalar, z * scalar);
    }
    
    IVector3 operator/(T scalar) const {
        return IVector3(x / scalar, y / scalar, z / scalar);
    }
    
    // Operadores de comparación
    bool operator==(const IVector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const IVector3& other) const {
        return !(*this == other);
    }
    
    // Métodos matemáticos
    float length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    IVector3<float> normalize() const {
        float len = length();
        if (len == 0.0f) return IVector3<float>(0, 0, 0);
        return IVector3<float>(x/len, y/len, z/len);
    }
    
    float dot(const IVector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    IVector3 cross(const IVector3& other) const {
        return IVector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    // Conversión a IVector2
    IVector2<T> toIVector2() const {
        return IVector2<T>(x, y);
    }
    
    // Serialización con DataBuffer
    void serialize(DataBuffer& buffer) const {
        buffer << x << y << z;
    }
    
    void deserialize(DataBuffer& buffer) {
        buffer >> x >> y >> z;
    }
    
    // Conversión a string
    std::string toString() const {
        return "IVector3(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

// Operadores globales para escalares
template<typename T>
IVector3<T> operator*(T scalar, const IVector3<T>& vec) {
    return vec * scalar;
}

// Integración con ThreadSafeIOStream
template<typename T>
ThreadSafeIOStream& operator<<(ThreadSafeIOStream& stream, const IVector3<T>& vec) {
    stream << vec.toString();
    return stream;
}

// Integración con DataBuffer
template<typename T>
DataBuffer& operator<<(DataBuffer& buffer, const IVector3<T>& vec) {
    vec.serialize(buffer);
    return buffer;
}

template<typename T>
DataBuffer& operator>>(DataBuffer& buffer, IVector3<T>& vec) {
    vec.deserialize(buffer);
    return buffer;
}

#endif // IVECTOR3_HPP