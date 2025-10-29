#ifndef THREAD_HPP
#define THREAD_HPP

#include <thread>
#include <string>
#include <functional>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include "iostreams/thread_safe_iostream.hpp"

/**
 * @class Thread
 * @brief Wrapper simple y funcional para std::thread con nombre
 * 
 * Cumple con los requisitos del subject:
 * - Constructor con nombre y función
 * - Métodos start() y stop()
 * - Integración con ThreadSafeIOStream para prefijos
 */
class Thread {
private:
    std::string _name;
    std::function<void()> _function;
    std::thread _thread;
    std::mutex _mutex;
    std::atomic<bool> _running;
    std::atomic<bool> _shouldStop;

public:
    /**
     * @brief Constructor que configura el hilo con nombre y función
     * @param name Nombre del hilo (usado como prefijo en ThreadSafeIOStream)
     * @param functToExecute Función a ejecutar en el hilo
     */
    Thread(const std::string& name, std::function<void()> functToExecute);
    
    /**
     * @brief Destructor - asegura la parada segura del hilo
     */
    ~Thread();

    /**
     * @brief Inicia la ejecución del hilo
     * @throw std::runtime_error si el hilo ya está ejecutándose
     */
    void start();
    
    /**
     * @brief Detiene el hilo de manera controlada
     */
    void stop();

    /**
     * @brief Obtiene el nombre del hilo
     * @return Nombre del hilo
     */
    const std::string& getName() const;
    
    /**
     * @brief Verifica si el hilo está ejecutándose
     * @return true si está ejecutándose, false en caso contrario
     */
    bool isRunning() const;
    
    /**
     * @brief Verifica si se ha solicitado la parada del hilo
     * @return true si se ha llamado a stop(), false en caso contrario
     */
    bool shouldStop() const;

private:
    /**
     * @brief Función interna que ejecuta el hilo
     */
    void threadFunction();
};

#endif // THREAD_HPP