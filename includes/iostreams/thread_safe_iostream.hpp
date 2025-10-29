#ifndef THREAD_SAFE_IOSTREAM_HPP
#define THREAD_SAFE_IOSTREAM_HPP

#include <iostream>
#include <sstream>
#include <mutex>
#include <string>
#include <thread>
#include <functional>
#include <algorithm> // Incluir algorithm para transform
#include <cctype>    // Incluir cctype para tolower


// Incluir todas las dependencias del proyecto
#include "data_structures/data_buffer.hpp"
#include "data_structures/pool.hpp"
#include "design_patterns/memento.hpp"
#include "design_patterns/observer.hpp"
#include "design_patterns/singleton.hpp"
#include "design_patterns/state_machine.hpp"

// Estados para la máquina de estados del stream
enum class StreamState {
    IDLE,       ///< Estado inactivo, sin datos en buffer
    BUFFERING,  ///< Estado de almacenamiento en buffer  
    FLUSHING    ///< Estado de volcado a salida estándar
};

// Eventos para el Observer del stream
enum class StreamEvent {
    LINE_PRINTED,    ///< Se imprimió una línea
    PREFIX_CHANGED,  ///< Se cambió el prefijo
    STREAM_FLUSHED   ///< Se vació el stream
};

/**
 * @class ThreadSafeIOStream
 * @brief Versión thread-safe de iostream con prefijos y uso de patrones de diseño.
 */
class ThreadSafeIOStream {
private:
    // Recursos para gestión de estado y eventos
    StateMachine<StreamState> _stateMachine;      ///< Máquina de estados del stream
    Observer<StreamEvent, std::string> _observer; ///< Observer para eventos del stream
    
    // Recursos para gestión de memoria y datos
    Pool<std::string> _stringPool;               ///< Pool para reutilizar strings
    DataBuffer _dataBuffer;                      ///< Buffer para datos serializados
    
    // Sincronización - mutex estático para compartir entre todas las instancias
    static std::mutex _coutMutex;  ///< Mutex estático para sincronizar acceso a std::cout
    static std::mutex _cinMutex;   ///< Mutex estático para sincronizar acceso a std::cin

public:
    /**
     * @brief Constructor por defecto
     */
    ThreadSafeIOStream();
    
    /**
     * @brief Destructor - asegura que todo el buffer se vacíe
     */
    ~ThreadSafeIOStream();

    // ============ CONFIGURACIÓN BÁSICA ============
    
    /**
     * @brief Establece el prefijo que se imprimirá antes de cada línea
     * @param prefix Cadena que se usará como prefijo
     */
    void setPrefix(const std::string& prefix);
    
    /**
     * @brief Obtiene el prefijo actual del hilo
     * @return Prefijo actual del hilo
     */
    std::string getPrefix() const;

    // ============ OPERADORES DE SALIDA ============
    
    /**
     * @brief Operador de inserción para tipos genéricos
     * @tparam T Tipo del dato a insertar
     * @param value Valor a insertar en el buffer
     * @return Referencia a este ThreadSafeIOStream para encadenamiento
     */
    template<typename T>
    ThreadSafeIOStream& operator<<(const T& value);
    
    /**
     * @brief Operador de inserción para manipuladores de stream
     * @param manip Manipulador de stream (endl, flush, etc.)
     * @return Referencia a este ThreadSafeIOStream para encadenamiento
     */
    ThreadSafeIOStream& operator<<(std::ostream& (*manip)(std::ostream&));

    // ============ OPERADORES DE ENTRADA ============
    
    /**
     * @brief Operador de extracción para tipos genéricos
     * @tparam T Tipo del dato a extraer
     * @param value Variable donde almacenar el dato leído
     * @return Referencia a este ThreadSafeIOStream para encadenamiento
     */
    template<typename T>
    ThreadSafeIOStream& operator>>(T& value);

    // ============ MÉTODOS ADICIONALES ============
    
    /**
     * @brief Muestra una pregunta y lee la respuesta del usuario
     * @tparam T Tipo del dato a leer
     * @param question Pregunta a mostrar al usuario
     * @param dest Variable donde almacenar la respuesta
     */
    template<typename T>
    void prompt(const std::string& question, T& dest);

    /**
     * @brief Vacía el buffer de salida inmediatamente
     */
    void flush();

    // ============ MÉTODOS DE PATRONES DE DISEÑO ============
    
    /**
     * @brief Suscribe un callback a eventos del stream
     * @param event Evento al cual suscribirse
     * @param callback Función a ejecutar cuando ocurra el evento
     */
    void subscribeToEvent(StreamEvent event, const std::function<void(const std::string&)>& callback);
    
    /**
     * @brief Guarda el estado actual del stream en un Memento
     * @return Snapshot con el estado serializado del stream
     */
    Memento::Snapshot saveState();
    
    /**
     * @brief Restaura el estado del stream desde un Memento
     * @param snapshot Snapshot con el estado a restaurar
     */
    void restoreState(const Memento::Snapshot& snapshot);

    /**
     * @brief Obtiene el estado actual de la máquina de estados
     * @return Estado actual del stream
     */
    StreamState getCurrentState() const;

private:
    /**
     * @brief Inicializa la máquina de estados con los estados válidos
     */
    void initializeStateMachine();

    /**
     * @brief Obtiene el buffer local del hilo actual
     * @return Referencia al buffer local del hilo
     */
    std::ostringstream& getLocalBuffer();
    
    /**
     * @brief Obtiene el prefijo local del hilo actual
     * @return Referencia al prefijo local del hilo
     */
    std::string& getLocalPrefix();
};

/**
 * @brief Instancia global thread-safe equivalente a std::cout
 * 
 * Esta variable global proporciona una instancia ÚNICA de ThreadSafeIOStream
 * que puede ser usada por todos los hilos de manera thread-safe.
 */
extern ThreadSafeIOStream threadSafeCout;

// Incluir implementaciones de templates
#include "thread_safe_iostream.tpp"

#endif // THREAD_SAFE_IOSTREAM_HPP