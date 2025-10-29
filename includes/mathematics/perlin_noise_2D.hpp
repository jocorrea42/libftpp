// includes/mathematics/perlin_noise_2D.hpp
#ifndef PERLIN_NOISE_2D_HPP
#define PERLIN_NOISE_2D_HPP

#include "design_patterns/state_machine.hpp"
#include "design_patterns/memento.hpp"
#include "data_structures/data_buffer.hpp"
#include "mathematics/random_2D_coordinate_generator.hpp"
#include "mathematics/ivector2.hpp"
#include <vector>
#include <cmath>

enum class NoiseState {
    IDLE,
    GENERATING_GRADIENTS,
    INTERPOLATING,
    FINISHED
};

class PerlinNoise2D : public Memento {
private:
    Random2DCoordinateGenerator _rng;
    StateMachine<NoiseState> _stateMachine;
    DataBuffer _gradientBuffer;
    std::vector<int> _permutation;
    
    // Métodos Memento
    void _saveToSnapshot(Memento::Snapshot& snapshot) const override {
        snapshot << _rng.seed();
        snapshot.save_string(_gradientBuffer.str());
    }
    
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override {
        long long seed;
        snapshot >> seed;
        _rng.setSeed(seed);
        
        std::string gradientData;
        snapshot.load_string(gradientData);
        _gradientBuffer.clear();
        _gradientBuffer.append(gradientData);
        
        initializePermutation();
    }
    
    // Inicialización de la permutación
    void initializePermutation() {
        _permutation.resize(512);
        for (int i = 0; i < 256; ++i) {
            _permutation[i] = _rng(i, 0) % 256;
            _permutation[256 + i] = _permutation[i];
        }
    }
    
    // Métodos internos de Perlin
    float fade(float t) const {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
    float lerp(float a, float b, float t) const {
        return a + t * (b - a);
    }
    
    float grad(int hash, float x, float y) const {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    PerlinNoise2D(long long seed = 0) 
        : _rng(seed), _stateMachine(), _gradientBuffer() {
        
        initializeStateMachine();
        initializePermutation();
    }
    
    void initializeStateMachine() {
        // Agregar todos los estados
        _stateMachine.addState(NoiseState::IDLE);
        _stateMachine.addState(NoiseState::GENERATING_GRADIENTS);
        _stateMachine.addState(NoiseState::INTERPOLATING);
        _stateMachine.addState(NoiseState::FINISHED);
        
        // 🔥 CORRECCIÓN: Definir solo las transiciones lógicas necesarias
        
        // Desde IDLE solo podemos ir a GENERATING_GRADIENTS
        _stateMachine.addTransition(NoiseState::IDLE, NoiseState::GENERATING_GRADIENTS,
            []() { /* threadSafeCout << "[Perlin] Starting gradient generation..." << std::endl; */ });
        
        // Desde GENERATING_GRADIENTS podemos ir a INTERPOLATING o FINISHED
        _stateMachine.addTransition(NoiseState::GENERATING_GRADIENTS, NoiseState::INTERPOLATING,
            []() { /* threadSafeCout << "[Perlin] Gradients ready, starting interpolation..." << std::endl; */ });
        _stateMachine.addTransition(NoiseState::GENERATING_GRADIENTS, NoiseState::FINISHED,
            []() { /* threadSafeCout << "[Perlin] Gradient generation completed" << std::endl; */ });
        
        // Desde INTERPOLATING podemos volver a IDLE (para más samples) o a FINISHED
        _stateMachine.addTransition(NoiseState::INTERPOLATING, NoiseState::IDLE,
            []() { /* threadSafeCout << "[Perlin] Interpolation completed, ready for more" << std::endl; */ });
        _stateMachine.addTransition(NoiseState::INTERPOLATING, NoiseState::FINISHED,
            []() { /* threadSafeCout << "[Perlin] Final interpolation completed" << std::endl; */ });
        
        // Desde FINISHED podemos resetear a IDLE
        _stateMachine.addTransition(NoiseState::FINISHED, NoiseState::IDLE,
            []() { /* threadSafeCout << "[Perlin] Resetting to idle" << std::endl; */ });
        
        // 🔥 CORRECCIÓN: Establecer estado inicial explícitamente
        // No usar transitionTo() aquí, establecer directamente el estado inicial
        // ya que no hay transición definida para el estado inicial
    }
    
    float sample(float x, float y) {
        // Solo generar gradientes si estamos en IDLE
        if (_stateMachine.getCurrentState() == NoiseState::IDLE) {
            if (_stateMachine.canTransitionTo(NoiseState::GENERATING_GRADIENTS)) {
                _stateMachine.transitionTo(NoiseState::GENERATING_GRADIENTS);
            }
        }
        
        // Una vez que los gradientes están listos, podemos interpolar
        if (_stateMachine.getCurrentState() == NoiseState::GENERATING_GRADIENTS) {
            if (_stateMachine.canTransitionTo(NoiseState::INTERPOLATING)) {
                _stateMachine.transitionTo(NoiseState::INTERPOLATING);
            }
        }
        
        // Cálculo del noise (mismo que antes)
        int X = (int)std::floor(x) & 255;
        int Y = (int)std::floor(y) & 255;
        
        x -= std::floor(x);
        y -= std::floor(y);
        
        float u = fade(x);
        float v = fade(y);
        
        int aa = _permutation[_permutation[X] + Y];
        int ab = _permutation[_permutation[X] + Y + 1];
        int ba = _permutation[_permutation[X + 1] + Y];
        int bb = _permutation[_permutation[X + 1] + Y + 1];
        
        float result = lerp(
            lerp(grad(aa, x, y), grad(ba, x - 1, y), u),
            lerp(grad(ab, x, y - 1), grad(bb, x - 1, y - 1), u),
            v
        );
        
        // Después de interpolar, volver a IDLE para más samples
        if (_stateMachine.getCurrentState() == NoiseState::INTERPOLATING) {
            if (_stateMachine.canTransitionTo(NoiseState::IDLE)) {
                _stateMachine.transitionTo(NoiseState::IDLE);
            }
        }
        
        return result;
    }
    
    // Generación de área usando StateMachine
    std::vector<std::vector<float>> generateArea(const IVector2<float>& start, 
                                                const IVector2<float>& end, 
                                                const IVector2<int>& resolution) {
        
        // Comenzar desde IDLE
        if (_stateMachine.getCurrentState() == NoiseState::IDLE) {
            if (_stateMachine.canTransitionTo(NoiseState::GENERATING_GRADIENTS)) {
                _stateMachine.transitionTo(NoiseState::GENERATING_GRADIENTS);
            }
        }
        
        std::vector<std::vector<float>> noiseMap;
        float stepX = (end.x - start.x) / resolution.x;
        float stepY = (end.y - start.y) / resolution.y;
        
        for (int i = 0; i <= resolution.x; ++i) {
            std::vector<float> row;
            for (int j = 0; j <= resolution.y; ++j) {
                float x = start.x + i * stepX;
                float y = start.y + j * stepY;
                
                // Usar sample() que maneja las transiciones automáticamente
                float value = sample(x, y);
                row.push_back(value);
            }
            noiseMap.push_back(row);
        }
        
        // Terminar el proceso
        if (_stateMachine.getCurrentState() == NoiseState::IDLE) {
            if (_stateMachine.canTransitionTo(NoiseState::FINISHED)) {
                _stateMachine.transitionTo(NoiseState::FINISHED);
            }
        }
        
        return noiseMap;
    }
    
    // Estado actual
    NoiseState getCurrentState() const {
        return _stateMachine.getCurrentState();
    }
    
    // Cambiar semilla
    void setSeed(long long seed) {
        _rng.setSeed(seed);
        initializePermutation();
    }
    
    // 🔥 NUEVO: Método para inicializar/resetear el estado
    void reset() {
        // Reiniciar la máquina de estados
        _stateMachine = StateMachine<NoiseState>();
        initializeStateMachine();
    }
};

#endif // PERLIN_NOISE_2D_HPP