// includes/mathematics/random_2D_coordinate_generator.hpp
#ifndef RANDOM_2D_COORDINATE_GENERATOR_HPP
#define RANDOM_2D_COORDINATE_GENERATOR_HPP

#include "data_structures/pool.hpp"
#include "design_patterns/observer.hpp"
#include "mathematics/ivector2.hpp"
#include <functional>
#include <vector>
#include <string>

enum class GenerationEvent {
    VALUE_GENERATED,
    BATCH_COMPLETED,
    SEED_CHANGED
};

class Random2DCoordinateGenerator {
private:
    long long _seed;
    Pool<long long> _valuePool;
    Observer<GenerationEvent, long long> _generationObserver;
    
    // Función de hash determinista basada en coordenadas
    long long hashCoordinates(long long x, long long y) const {
        // Algoritmo de hash mejorado
        long long hash = _seed;
        hash = (hash << 13) ^ hash;
        hash = (hash * 0x9e3779b9) ^ (x * 0x85ebca6b);
        hash = (hash * 0x9e3779b9) ^ (y * 0xc2b2ae35);
        hash = (hash << 13) ^ hash;
        return hash & 0x7FFFFFFFFFFFFFFF; // Solo números positivos
    }

public:
    Random2DCoordinateGenerator(long long seed = 0) 
        : _seed(seed), _valuePool(), _generationObserver() {
        
        // Inicializar pool con valores precalculados
        _valuePool.resize(1000);
    }
    
    long long seed() const { return _seed; }
    
    void setSeed(long long newSeed) {
        _seed = newSeed;
        _generationObserver.notify(GenerationEvent::SEED_CHANGED, _seed);
    }
    
    // Operador de función - generación individual
    long long operator()(const long long& x, const long long& y) {
        long long value = hashCoordinates(x, y);
        _generationObserver.notify(GenerationEvent::VALUE_GENERATED, value);
        return value;
    }
    
    // Generación por lotes - VERSIÓN CORREGIDA
    std::vector<long long> generateBatch(const IVector2<long long>& start, 
                                        const IVector2<long long>& end) {
        
        std::vector<long long> results;
        long long count = 0;
        
        for (long long x = start.x; x <= end.x; ++x) {
            for (long long y = start.y; y <= end.y; ++y) {
                // Generar valor directamente sin usar el Pool para long long
                long long value = (*this)(x, y);
                results.push_back(value);
                count++;
            }
        }
        
        _generationObserver.notify(GenerationEvent::BATCH_COMPLETED, count);
        return results;
    }
    
    // Versión alternativa que SÍ usa el Pool (si quieres mantenerla)
    std::vector<long long> generateBatchWithPool(const IVector2<long long>& start, 
                                                const IVector2<long long>& end) {
        
        std::vector<long long> results;
        long long count = 0;
        
        for (long long x = start.x; x <= end.x; ++x) {
            for (long long y = start.y; y <= end.y; ++y) {
                // Usar el pool correctamente
                auto valueObj = _valuePool.acquire((*this)(x, y));
                // Usar operator*() que acabamos de agregar
                results.push_back(*valueObj); // ✅ AHORA FUNCIONA
                count++;
            }
        }
        
        _generationObserver.notify(GenerationEvent::BATCH_COMPLETED, count);
        return results;
    }
    
    // Suscripción a eventos
    void subscribeToGeneration(GenerationEvent event, 
                              const std::function<void(long long)>& callback) {
        _generationObserver.subscribe(event, callback);
    }
    
    // Serialización del estado
    void serializeState(DataBuffer& buffer) const {
        buffer << _seed;
    }
    
    void deserializeState(DataBuffer& buffer) {
        buffer >> _seed;
    }
};

#endif // RANDOM_2D_COORDINATE_GENERATOR_HPP