#include "threading/thread.hpp"
#include <chrono>
#include <iostream>

// ============ CONSTRUCTOR Y DESTRUCTOR ============

Thread::Thread(const std::string& name, std::function<void()> functToExecute)
    : _name(name), 
      _function(functToExecute), 
      _running(false),
      _shouldStop(false) {
    
    // Validaciones básicas
    if (_name.empty()) {
        throw std::invalid_argument("Thread name cannot be empty");
    }
    
    if (!_function) {
        throw std::invalid_argument("Thread function cannot be null");
    }
}

Thread::~Thread() {
    stop(); // Asegura la parada segura al destruir
}

// ============ MÉTODOS PÚBLICOS PRINCIPALES ============

void Thread::start() {
    std::lock_guard<std::mutex> lock(_mutex);
    
    if (_running) {
        throw std::runtime_error("Thread is already running");
    }
    
    if (_thread.joinable()) {
        throw std::runtime_error("Thread is still joinable");
    }
    
    _running = true;
    _shouldStop = false;
    
    // Lanzar el hilo
    _thread = std::thread(&Thread::threadFunction, this);
}

void Thread::stop() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_running) {
            return; // Ya está detenido
        }
        
        _shouldStop = true;
        _running = false;
    }
    
    // Esperar a que el hilo termine
    if (_thread.joinable()) {
        _thread.join();
    }
}

// ============ MÉTODOS DE CONSULTA ============

const std::string& Thread::getName() const {
    return _name;
}

bool Thread::isRunning() const {
    return _running.load();
}

bool Thread::shouldStop() const {
    return _shouldStop.load();
}

// ============ FUNCIÓN DE EJECUCIÓN DEL HILO ============

void Thread::threadFunction() {
    try {
        // Establecer el nombre del hilo como prefijo en ThreadSafeIOStream
        threadSafeCout.setPrefix("[" + _name + "] ");
        
        // Ejecutar la función del usuario
        _function();
        
    } catch (const std::exception& e) {
        // En una implementación simple, solo capturamos la excepción
        // pero no hacemos nada para no complicar el código
        (void)e; // Evitar warning de variable no usada
    } catch (...) {
        // Capturar cualquier otra excepción
    }
}