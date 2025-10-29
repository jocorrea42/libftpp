// includes/mathematics/mathematics_singleton.hpp
#ifndef MATHEMATICS_SINGLETON_HPP
#define MATHEMATICS_SINGLETON_HPP

#include "design_patterns/singleton.hpp"
#include "data_structures/pool.hpp"
#include "mathematics/ivector2.hpp"
#include "mathematics/ivector3.hpp"
#include "mathematics/random_2D_coordinate_generator.hpp"

class MathematicsSingleton {
private:
    static MathematicsSingleton* _instance;
    Random2DCoordinateGenerator _defaultRNG;
    Pool<IVector2<float>> _vector2Pool;
    Pool<IVector3<float>> _vector3Pool;
    
    MathematicsSingleton() 
        : _defaultRNG(42), _vector2Pool(), _vector3Pool() {
        
        // Inicializar pools con capacidad inicial
        _vector2Pool.resize(100);
        _vector3Pool.resize(100);
    }

public:
    static MathematicsSingleton* instance() {
        if (!_instance) {
            _instance = new MathematicsSingleton();
        }
        return _instance;
    }
    
    // Acceso a recursos compartidos
    Random2DCoordinateGenerator& getDefaultRNG() {
        return _defaultRNG;
    }
    
    // Pool de vectores para reutilización
    IVector2<float> acquireVector2(float x = 0.0f, float y = 0.0f) {
        auto vecObj = _vector2Pool.acquire(x, y);
        return *vecObj;
    }
    
    IVector3<float> acquireVector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
        auto vecObj = _vector3Pool.acquire(x, y, z);
        return *vecObj;
    }
    
    void releaseVector2(IVector2<float>& vec) {
        // En una implementación real, necesitaríamos trackear qué objeto del pool es
        // Por simplicidad, en esta implementación no hacemos nada
        (void)vec; // Evitar warning
    }
    
    void releaseVector3(IVector3<float>& vec) {
        (void)vec; // Evitar warning
    }
    
    // Destrucción controlada
    static void destroy() {
        if (_instance) {
            delete _instance;
            _instance = nullptr;
        }
    }
    
    // Eliminar copia y asignación
    MathematicsSingleton(const MathematicsSingleton&) = delete;
    MathematicsSingleton& operator=(const MathematicsSingleton&) = delete;
};

// Inicialización de la variable estática
MathematicsSingleton* MathematicsSingleton::_instance = nullptr;

#endif // MATHEMATICS_SINGLETON_HPP