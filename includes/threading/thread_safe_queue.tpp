#ifndef THREAD_SAFE_QUEUE_TPP
#define THREAD_SAFE_QUEUE_TPP

#include "thread_safe_queue.hpp"
#include <chrono>
#include <sstream>

// ============ CONSTRUCTOR Y DESTRUCTOR ============

/**
 * @brief Constructor - inicializa la cola y configura todos los componentes
 * 
 * INICIALIZACIÓN:
 * - _closed = false (cola abierta)
 * - StateMachine con estados y transiciones
 * - Observer listo para recibir suscripciones
 * - DataBuffer vacío listo para logging
 */
template <typename TType>
ThreadSafeQueue<TType>::ThreadSafeQueue() : _closed(false) {
    initializeStateMachine();
}

/**
 * @brief Destructor - garantiza limpieza segura
 * 
 * GARANTÍAS:
 * - Cierra la cola para prevenir nuevas operaciones
 * - Notifica a todos los hilos esperando
 * - Previene condiciones de carrera durante la destrucción
 * - Libera todos los recursos correctamente
 */
template <typename TType>
ThreadSafeQueue<TType>::~ThreadSafeQueue() {
    close();
}

// ============ INICIALIZACIÓN DE STATE MACHINE ============

/**
 * @brief Configura la máquina de estados con todos los estados y transiciones
 * 
 * ESTADOS DEFINIDOS:
 * - EMPTY: Estado inicial, cola vacía
 * - NORMAL: Estado operativo normal, cola con elementos
 * - CLOSED: Estado final, cola cerrada
 * 
 * TRANSICIONES CONFIGURADAS:
 * - EMPTY → NORMAL: Cuando se inserta el primer elemento
 * - NORMAL → EMPTY: Cuando se extrae el último elemento
 * - CUALQUIER ESTADO → CLOSED: Cuando se cierra la cola
 * - CLOSED → EMPTY/NORMAL: Para compatibilidad con restauración
 */
template <typename TType>
void ThreadSafeQueue<TType>::initializeStateMachine() {
    // ============ CONFIGURACIÓN DE ESTADOS ============
    
    // Estado EMPTY: cola vacía, operaciones pop lanzarán excepción
    _stateMachine.addState(QueueState::EMPTY);
    
    // Estado NORMAL: cola con elementos, operaciones normales
    _stateMachine.addState(QueueState::NORMAL); 
    
    // Estado CLOSED: cola cerrada, operaciones push lanzarán excepción
    _stateMachine.addState(QueueState::CLOSED);
    
    // ============ CONFIGURACIÓN DE TRANSICIONES ============
    
    // Transición EMPTY → NORMAL: ocurre al insertar el primer elemento
    _stateMachine.addTransition(QueueState::EMPTY, QueueState::NORMAL, []() {
        // Callback vacío - puede ser personalizado para logging específico
    });
    
    // Transición NORMAL → EMPTY: ocurre al extraer el último elemento
    _stateMachine.addTransition(QueueState::NORMAL, QueueState::EMPTY, []() {
        // Callback vacío - puede ser personalizado para logging específico
    });
    
    // Transición NORMAL → CLOSED: ocurre al cerrar una cola con elementos
    _stateMachine.addTransition(QueueState::NORMAL, QueueState::CLOSED, []() {
        // Callback vacío - puede ser personalizado para logging específico
    });
    
    // Transición EMPTY → CLOSED: ocurre al cerrar una cola vacía
    _stateMachine.addTransition(QueueState::EMPTY, QueueState::CLOSED, []() {
        // Callback vacío - puede ser personalizado para logging específico
    });
    
    // Transición CLOSED → EMPTY: para compatibilidad con restauración de estado
    _stateMachine.addTransition(QueueState::CLOSED, QueueState::EMPTY, []() {
        // Callback vacío - puede ser personalizado para logging específico
    });
    
    // Transición CLOSED → NORMAL: para compatibilidad con restauración de estado
    _stateMachine.addTransition(QueueState::CLOSED, QueueState::NORMAL, []() {
        // Callback vacío - puede ser personalizado para logging específico
    });
    
    // ✅ NOTA: StateMachine establece automáticamente el primer estado (EMPTY) como inicial
}

// ============ ACTUALIZACIÓN DE ESTADO ============

/**
 * @brief Actualiza el estado de la máquina basado en el estado actual de la cola
 * 
 * LÓGICA DE DECISIÓN DE ESTADO:
 * 1. Si _closed es true → estado CLOSED
 * 2. Si la cola está vacía → estado EMPTY  
 * 3. Si la cola tiene elementos → estado NORMAL
 * 
 * OPTIMIZACIÓN:
 * - Solo realiza la transición si el nuevo estado es diferente al actual
 * - Verifica que la transición sea posible antes de intentarla
 */
template <typename TType>
void ThreadSafeQueue<TType>::updateState() {
    // Determinar el nuevo estado basado en las condiciones actuales
    QueueState newState = _closed ? QueueState::CLOSED : 
                        (_queue.empty() ? QueueState::EMPTY : QueueState::NORMAL);
    
    // Realizar transición solo si es posible y necesaria
    if (_stateMachine.canTransitionTo(newState) && 
        _stateMachine.getCurrentState() != newState) {
        _stateMachine.transitionTo(newState);
    }
}

// ============ FUNCIONES HELPER PARA CONVERSIÓN ============

/**
 * @brief Convierte un valor a su representación string
 * @tparam T Tipo del valor a convertir
 * @param value Valor a convertir
 * @return Representación string del valor
 * 
 * UTILIZA:
 * - std::ostringstream para conversión genérica
 * - Operator<< del tipo T para la conversión
 * 
 * @note Requiere que el tipo T tenga operator<< definido
 */
template<typename T>
inline std::string valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

/**
 * @brief Especialización para std::string - evita conversión redundante
 * @param value String a "convertir"
 * @return El mismo string sin cambios
 * 
 * OPTIMIZACIÓN:
 * - Evita la creación innecesaria de std::ostringstream
 * - Retorna el string original directamente
 */
template<>
inline std::string valueToString<std::string>(const std::string& value) {
    return value;
}

// ============ OPERACIONES DE INSERCIÓN ============

/**
 * @brief Inserta un elemento al final de la cola
 * 
 * FLUJO DE EJECUCIÓN:
 * 1. 🔒 Bloquea el mutex (lock_guard - liberación automática)
 * 2. 🚫 Verifica que la cola no esté cerrada
 * 3. 📦 Inserta el elemento al final del deque
 * 4. 📝 Registra la operación en DataBuffer
 * 5. 🔄 Actualiza el estado de la StateMachine
 * 6. 🔔 Notifica a un consumidor (si está esperando)
 * 7. 📢 Emite evento ELEMENT_PUSHED a los observadores
 */
template <typename TType>
void ThreadSafeQueue<TType>::push_back(const TType& newElement) {
    // Paso 1: Sincronización thread-safe
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Paso 2: Validación de estado
    if (_closed) {
        throw std::runtime_error("Cannot push to closed queue");
    }
    
    // Paso 3: Operación principal
    _queue.push_back(newElement);
    
    // Paso 4: Logging de la operación
    _dataBuffer.append("Push back: " + valueToString(newElement) + "\n");
    
    // Paso 5: Actualización de estado
    updateState();
    
    // Paso 6: Notificación a consumidores
    _cv.notify_one();
    
    // Paso 7: Notificación a observadores
    _eventObserver.notify(QueueEvent::ELEMENT_PUSHED);
}

/**
 * @brief Inserta un elemento al frente de la cola
 * 
 * FLUJO DE EJECUCIÓN:
 * 1. 🔒 Bloquea el mutex (lock_guard - liberación automática)
 * 2. 🚫 Verifica que la cola no esté cerrada
 * 3. 📦 Inserta el elemento al frente del deque
 * 4. 📝 Registra la operación en DataBuffer
 * 5. 🔄 Actualiza el estado de la StateMachine
 * 6. 🔔 Notifica a un consumidor (si está esperando)
 * 7. 📢 Emite evento ELEMENT_PUSHED a los observadores
 */
template <typename TType>
void ThreadSafeQueue<TType>::push_front(const TType& newElement) {
    // Paso 1: Sincronización thread-safe
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Paso 2: Validación de estado
    if (_closed) {
        throw std::runtime_error("Cannot push to closed queue");
    }
    
    // Paso 3: Operación principal
    _queue.push_front(newElement);
    
    // Paso 4: Logging de la operación
    _dataBuffer.append("Push front: " + valueToString(newElement) + "\n");
    
    // Paso 5: Actualización de estado
    updateState();
    
    // Paso 6: Notificación a consumidores
    _cv.notify_one();
    
    // Paso 7: Notificación a observadores
    _eventObserver.notify(QueueEvent::ELEMENT_PUSHED);
}

// ============ IMPLEMENTACIÓN COMÚN PARA OPERACIONES POP ============

/**
 * @brief Implementación común para operaciones de extracción
 * 
 * ESTA IMPLEMENTACIÓN CENTRALIZA:
 * - ✅ Sincronización thread-safe
 * - ✅ Verificación de cola vacía
 * - ✅ Logging consistente
 * - ✅ Actualización de estado
 * - ✅ Notificación de eventos
 * 
 * FLUJO DE EJECUCIÓN:
 * 1. 🔒 Bloquea el mutex (lock_guard - liberación automática)
 * 2. 🚫 Verifica si la cola está vacía → lanza EmptyQueueException
 * 3. 📦 Ejecuta la operación específica (back o front) mediante el functor
 * 4. 📝 Registra la operación en DataBuffer
 * 5. 🔄 Actualiza el estado de la StateMachine
 * 6. 📢 Emite evento ELEMENT_POPPED a los observadores
 * 7. 📤 Retorna el elemento extraído
 */
template <typename TType>
template<typename Operation>
TType ThreadSafeQueue<TType>::pop_impl(Operation operation, const std::string& operation_name) {
    // Paso 1: Sincronización thread-safe
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Paso 2: Verificación de cola vacía
    if (_queue.empty()) {
        // Logging de operación fallida
        _dataBuffer.append("Pop failed: queue empty\n");
        
        // Notificación de evento (aunque la operación falló)
        _eventObserver.notify(QueueEvent::ELEMENT_POPPED);
        
        // Lanzar excepción específica con mensaje claro
        throw EmptyQueueException();
    }
    
    // Paso 3: Ejecutar operación específica mediante el functor
    TType value = std::move(operation());
    
    // Paso 4: Logging de operación exitosa
    _dataBuffer.append(operation_name + ": " + valueToString(value) + "\n");
    
    // Paso 5: Actualización de estado
    updateState();
    
    // Paso 6: Notificación a observadores
    _eventObserver.notify(QueueEvent::ELEMENT_POPPED);
    
    // Paso 7: Retornar valor
    return value;
}

// ============ OPERACIONES DE EXTRACCIÓN ESPECÍFICAS ============

/**
 * @brief Extrae el último elemento de la cola
 * 
 * UTILIZA la implementación común con un functor específico para pop_back
 * 
 * @return Último elemento de la cola
 * @throw EmptyQueueException si la cola está vacía
 */
template <typename TType>
TType ThreadSafeQueue<TType>::pop_back() {
    return pop_impl([this]() {
        // Functor que realiza la operación específica de pop_back
        TType value = std::move(_queue.back());
        _queue.pop_back();
        return value;
    }, "Pop back");
}

/**
 * @brief Extrae el primer elemento de la cola
 * 
 * UTILIZA la implementación común con un functor específico para pop_front
 * 
 * @return Primer elemento de la cola
 * @throw EmptyQueueException si la cola está vacía
 */
template <typename TType>
TType ThreadSafeQueue<TType>::pop_front() {
    return pop_impl([this]() {
        // Functor que realiza la operación específica de pop_front
        TType value = std::move(_queue.front());
        _queue.pop_front();
        return value;
    }, "Pop front");
}

// ============ MÉTODOS DE CONSULTA ============

/**
 * @brief Verifica si la cola está vacía
 * 
 * CARACTERÍSTICAS:
 * - Operación thread-safe
 * - Proporciona instantánea del estado actual
 * - No modifica el estado de la cola
 * 
 * @return true si la cola no contiene elementos, false en caso contrario
 */
template <typename TType>
bool ThreadSafeQueue<TType>::empty() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.empty();
}

/**
 * @brief Obtiene el número de elementos en la cola
 * 
 * CARACTERÍSTICAS:
 * - Operación thread-safe
 * - Proporciona instantánea del estado actual
 * - No modifica el estado de la cola
 * 
 * @return Cantidad de elementos en la cola
 */
template <typename TType>
size_t ThreadSafeQueue<TType>::size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.size();
}

// ============ MÉTODOS DE GESTIÓN ============

/**
 * @brief Cierra la cola para nuevas inserciones
 * 
 * EFECTOS:
 * - Establece _closed = true
 * - Notifica a TODOS los hilos esperando en condition_variable
 * - Las operaciones push futuras lanzarán excepción
 * - Las operaciones pop existentes pueden continuar
 * 
 * USO TÍPICO:
 * - Para shutdown ordenado de la aplicación
 * - Para prevenir nuevas inserciones mientras se vacía la cola
 */
template <typename TType>
void ThreadSafeQueue<TType>::close() {
    std::lock_guard<std::mutex> lock(_mutex);
    _closed = true;
    _cv.notify_all();
}

/**
 * @brief Verifica si la cola está cerrada
 * 
 * @return true si la cola está cerrada, false si está abierta
 */
template <typename TType>
bool ThreadSafeQueue<TType>::is_closed() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _closed;
}

// ============ IMPLEMENTACIÓN DEL PATRÓN MEMENTO ============

/**
 * @brief Guarda el estado actual de la cola en un snapshot
 * 
 * DATOS SERIALIZADOS:
 * - Estado de cierre (_closed)
 * - Contenido completo del DataBuffer (historial de operaciones)
 * - Cantidad de elementos en la cola
 * - Todos los elementos individuales de la cola
 * 
 * REQUISITOS:
 * - El tipo TType debe ser serializable (tener operator<<)
 * - Todas las operaciones son thread-safe
 * 
 * @param snapshot Referencia al snapshot donde se guardará el estado
 */
template <typename TType>
void ThreadSafeQueue<TType>::_saveToSnapshot(Memento::Snapshot& snapshot) const {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Guardar estado de cierre
    snapshot << _closed;
    
    // Guardar contenido del DataBuffer (historial de operaciones)
    snapshot.save_string(_dataBuffer.str());
    
    // Guardar cantidad de elementos
    size_t size = _queue.size();
    snapshot << size;
    
    // Guardar cada elemento individualmente
    for (const auto& item : _queue) {
        snapshot << item;
    }
}

/**
 * @brief Restaura el estado de la cola desde un snapshot
 * 
 * OPERACIONES REALIZADAS:
 * - Restaura el estado de cierre
 * - Restaura el historial de operaciones del DataBuffer
 * - Reconstruye todos los elementos de la cola
 * - Actualiza la StateMachine al estado correspondiente
 * - Notifica a todos los hilos esperando
 * 
 * @param snapshot Referencia al snapshot desde donde se restaurará el estado
 */
template <typename TType>
void ThreadSafeQueue<TType>::_loadFromSnapshot(Memento::Snapshot& snapshot) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    // Restaurar estado de cierre
    snapshot >> _closed;
    
    // Restaurar contenido del DataBuffer
    std::string bufferContents;
    snapshot.load_string(bufferContents);
    _dataBuffer.clear();
    _dataBuffer.append(bufferContents);
    
    // Restaurar cantidad de elementos
    size_t size;
    snapshot >> size;
    
    // Limpiar cola actual y reconstruir desde snapshot
    _queue.clear();
    for (size_t i = 0; i < size; ++i) {
        TType item;
        snapshot >> item;
        _queue.push_back(item);
    }
    
    // Actualizar StateMachine al estado correspondiente
    updateState();
    
    // Notificar a todos los hilos esperando
    _cv.notify_all();
}

#endif // THREAD_SAFE_QUEUE_TPP