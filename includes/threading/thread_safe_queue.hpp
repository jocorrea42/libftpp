#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <deque>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include "data_structures/data_buffer.hpp"
#include "design_patterns/memento.hpp"
#include "design_patterns/singleton.hpp"
#include "design_patterns/state_machine.hpp"
#include "design_patterns/observer.hpp"

/**
 * @enum QueueState
 * @brief Enumeración que define los estados posibles de la cola thread-safe
 * 
 * Esta enumeración es utilizada por la StateMachine para gestionar el ciclo
 * de vida de la cola y las transiciones entre diferentes estados operativos.
 */
enum class QueueState {
    EMPTY,      ///< Estado vacío: la cola no contiene elementos. Operaciones pop lanzarán excepción.
    NORMAL,     ///< Estado normal: la cola contiene elementos y acepta operaciones normales.
    CLOSED      ///< Estado cerrado: la cola no acepta nuevas inserciones. Operaciones push lanzarán excepción.
};

/**
 * @enum QueueEvent  
 * @brief Enumeración que define los eventos que puede emitir la cola
 * 
 * Estos eventos son utilizados por el patrón Observer para notificar a los
 * suscriptores sobre cambios significativos en el estado de la cola.
 */
enum class QueueEvent {
    ELEMENT_PUSHED,    ///< Evento emitido cuando se inserta un nuevo elemento en la cola
    ELEMENT_POPPED     ///< Evento emitido cuando se extrae un elemento de la cola
};

/**
 * @class ThreadSafeQueue
 * @brief Cola thread-safe avanzada que integra múltiples patrones de diseño
 * 
 * Esta clase implementa una cola doble (deque) completamente thread-safe que
 * incorpora patrones de diseño avanzados para proporcionar funcionalidades
 * robustas en entornos multihilo complejos.
 * 
 * CARACTERÍSTICAS PRINCIPALES:
 * - ✅ Thread-safe completo con mutex y condition_variable
 * - ✅ Excepción inmediata en operaciones pop sobre cola vacía
 * - ✅ Integración con StateMachine para gestión de estados
 * - ✅ Notificación de eventos mediante patrón Observer
 * - ✅ Registro de operaciones en DataBuffer para logging
 * - ✅ Backup y restore de estado mediante patrón Memento
 * - ✅ Operaciones en ambos extremos (push/pop front/back)
 * 
 * PATRONES DE DISEÑO IMPLEMENTADOS:
 * - StateMachine: Gestión automática de estados (EMPTY, NORMAL, CLOSED)
 * - Observer: Notificación de eventos de operaciones
 * - DataBuffer: Registro y serialización de operaciones
 * - Memento: Snapshots del estado para persistencia
 * 
 * @tparam TType Tipo de los elementos almacenados en la cola. Debe ser serializable
 *               para el correcto funcionamiento del patrón Memento.
 * 
 * @example
 * ThreadSafeQueue<int> queue;
 * queue.push_back(42);
 * int value = queue.pop_front();
 * 
 * @example Uso con múltiples hilos:
 * ThreadSafeQueue<std::string> queue;
 * 
 * // Hilo productor
 * std::thread producer([&queue]() {
 *     queue.push_back("Hello");
 *     queue.push_back("World");
 * });
 * 
 * // Hilo consumidor  
 * std::thread consumer([&queue]() {
 *     try {
 *         std::string msg = queue.pop_front();
 *         std::cout << msg << std::endl;
 *     } catch (const ThreadSafeQueue<std::string>::EmptyQueueException& e) {
 *         std::cout << "Cola vacía" << std::endl;
 *     }
 * });
 */
template <typename TType>
class ThreadSafeQueue : public Memento {
private:
    // ============ MIEMBROS PRIVADOS ============
    
    std::deque<TType> _queue;              ///< Contenedor subyacente que almacena los elementos de la cola
    mutable std::mutex _mutex;             ///< Mutex para sincronización thread-safe de todas las operaciones
    std::condition_variable _cv;           ///< Variable de condición para coordinación entre hilos productores y consumidores
    bool _closed;                          ///< Flag que indica si la cola está cerrada para nuevas inserciones
    
    // ============ PATRONES DE DISEÑO INTEGRADOS ============
    
    StateMachine<QueueState> _stateMachine;    ///< Máquina de estados que gestiona el ciclo de vida de la cola
    Observer<QueueEvent, void> _eventObserver; ///< Sistema de observación para notificación de eventos
    DataBuffer _dataBuffer;                    ///< Buffer de datos para registro y logging de operaciones

public:
    // ============ MÉTODOS DE COMPATIBILIDAD PARA CLIENT/SERVER ============
    
    /**
     * @brief Inserta un elemento al final de la cola (alias de push_back para compatibilidad)
     * @param value Elemento a insertar
     * 
     * Este método proporciona compatibilidad con el código Client/Server que espera
     * un método push() simple en lugar de push_back().
     */
    void push(const TType& value) {
        push_back(value);
    }
    
    /**
     * @brief Intenta extraer un elemento del frente de la cola (alias seguro de pop_front)
     * @param value Referencia donde almacenar el elemento extraído
     * @return true si se extrajo un elemento, false si la cola estaba vacía
     * 
     * Este método proporciona compatibilidad con el código Client/Server que espera
     * un método try_pop() que no lance excepciones cuando la cola está vacía.
     */
    bool try_pop(TType& value) {
        if (empty()) {
            return false;
        }
        
        try {
            value = pop_front();
            return true;
        } catch (const EmptyQueueException&) {
            return false;
        }
    }
    
    /**
     * @brief Intenta extraer un elemento del frente de la cola (versión con shared_ptr)
     * @return shared_ptr al elemento extraído, o nullptr si la cola estaba vacía
     * 
     * Este método proporciona compatibilidad con código que prefiere usar smart pointers
     * para la gestión de memoria.
     */
    std::shared_ptr<TType> try_pop() {
        if (empty()) {
            return nullptr;
        }
        
        try {
            return std::make_shared<TType>(pop_front());
        } catch (const EmptyQueueException&) {
            return nullptr;
        }
    }
    // ============ EXCEPCIÓN PERSONALIZADA ============
    
    /**
     * @class EmptyQueueException
     * @brief Excepción lanzada cuando se intenta extraer un elemento de una cola vacía
     * 
     * Esta excepción proporciona un mensaje específico y claro indicando que la
     * operación no puede completarse porque la cola no contiene elementos.
     */
    class EmptyQueueException : public std::runtime_error {
    public:
        /**
         * @brief Constructor que establece el mensaje de error específico
         */
        EmptyQueueException() : std::runtime_error("ThreadSafeQueue: Queue is empty.") {}
    };

    // ============ CONSTRUCTOR Y DESTRUCTOR ============
    
    /**
     * @brief Constructor por defecto
     * 
     * Inicializa la cola en estado abierto y configura todos los patrones
     * de diseño integrados (StateMachine, Observer, DataBuffer).
     */
    ThreadSafeQueue();
    
    /**
     * @brief Destructor
     * 
     * Garantiza el cierre seguro de la cola y notifica a todos los hilos
     * que pudieran estar esperando en operaciones de extracción.
     */
    ~ThreadSafeQueue();

    // ============ MÉTODOS PRINCIPALES REQUERIDOS POR EL SUBJECT ============
    
    /**
     * @brief Inserta un elemento al final de la cola
     * @param newElement Elemento a insertar en la cola
     * @throw std::runtime_error si la cola está cerrada
     * 
     * OPERACIONES REALIZADAS:
     * 1. Bloquea el mutex para garantizar thread-safety
     * 2. Verifica que la cola no esté cerrada
     * 3. Inserta el elemento al final del deque
     * 4. Registra la operación en el DataBuffer
     * 5. Actualiza el estado de la StateMachine
     * 6. Notifica a un consumidor esperando (si existe)
     * 7. Emite evento ELEMENT_PUSHED a los observadores
     */
    void push_back(const TType& newElement);
    
    /**
     * @brief Inserta un elemento al frente de la cola
     * @param newElement Elemento a insertar en la cola
     * @throw std::runtime_error si la cola está cerrada
     * 
     * OPERACIONES REALIZADAS:
     * 1. Bloquea el mutex para garantizar thread-safety
     * 2. Verifica que la cola no esté cerrada
     * 3. Inserta el elemento al frente del deque
     * 4. Registra la operación en el DataBuffer
     * 5. Actualiza el estado de la StateMachine
     * 6. Notifica a un consumidor esperando (si existe)
     * 7. Emite evento ELEMENT_PUSHED a los observadores
     */
    void push_front(const TType& newElement);
    
    /**
     * @brief Extrae el último elemento de la cola
     * @return Elemento extraído del final de la cola
     * @throw EmptyQueueException si la cola está vacía
     * 
     * CARACTERÍSTICAS:
     * - Operación inmediata (sin esperas ni timeouts)
     * - Lanza excepción específica si la cola está vacía
     * - Utiliza movimiento (move semantics) para eficiencia
     * - Thread-safe completa
     */
    TType pop_back();
    
    /**
     * @brief Extrae el primer elemento de la cola
     * @return Elemento extraído del frente de la cola
     * @throw EmptyQueueException si la cola está vacía
     * 
     * CARACTERÍSTICAS:
     * - Operación inmediata (sin esperas ni timeouts)
     * - Lanza excepción específica si la cola está vacía
     * - Utiliza movimiento (move semantics) para eficiencia
     * - Thread-safe completa
     */
    TType pop_front();

    // ============ MÉTODOS DE CONSULTA ============
    
    /**
     * @brief Verifica si la cola está vacía
     * @return true si la cola no contiene elementos, false en caso contrario
     * 
     * @note Esta operación es thread-safe y proporciona una instantánea
     *       del estado en el momento de la consulta.
     */
    bool empty() const;
    
    /**
     * @brief Obtiene el número de elementos en la cola
     * @return Cantidad de elementos almacenados en la cola
     * 
     * @note Esta operación es thread-safe y proporciona una instantánea
     *       del estado en el momento de la consulta.
     */
    size_t size() const;

    // ============ MÉTODOS DE GESTIÓN AVANZADA ============
    
    /**
     * @brief Cierra la cola para nuevas inserciones
     * 
     * EFECTOS:
     * - Rechaza todas las operaciones push futuras
     * - Notifica a todos los hilos esperando en operaciones pop
     * - Cambia el estado de la StateMachine a CLOSED
     * 
     * @note Las operaciones pop existentes pueden continuar hasta que
     *       la cola quede vacía.
     */
    void close();
    
    /**
     * @brief Verifica si la cola está cerrada
     * @return true si la cola está cerrada, false si está abierta
     */
    bool is_closed() const;

private:
    // ============ MÉTODOS PRIVADOS ============
    
    /**
     * @brief Implementación del patrón Memento: guarda el estado actual
     * @param snapshot Referencia al snapshot donde se guardará el estado
     * 
     * DATOS SERIALIZADOS:
     * - Estado de cierre (_closed)
     * - Contenido completo del DataBuffer
     * - Cantidad de elementos en la cola
     * - Todos los elementos de la cola (requiere que TType sea serializable)
     */
    void _saveToSnapshot(Memento::Snapshot& snapshot) const override;
    
    /**
     * @brief Implementación del patrón Memento: restaura el estado
     * @param snapshot Referencia al snapshot desde donde se restaurará el estado
     * 
     * OPERACIONES REALIZADAS:
     * - Restaura el estado de cierre
     * - Restaura el contenido del DataBuffer
     * - Reconstruye todos los elementos de la cola
     * - Actualiza la StateMachine al estado correspondiente
     * - Notifica a todos los hilos esperando
     */
    void _loadFromSnapshot(Memento::Snapshot& snapshot) override;
    
    /**
     * @brief Inicializa la máquina de estados con estados y transiciones
     * 
     * CONFIGURACIÓN DE ESTADOS:
     * - EMPTY: Cola vacía
     * - NORMAL: Cola con elementos
     * - CLOSED: Cola cerrada
     * 
     * TRANSICIONES CONFIGURADAS:
     * - EMPTY ↔ NORMAL (inserciones/extracciones)
     * - CUALQUIER ESTADO → CLOSED (cierre de cola)
     * - CLOSED → EMPTY/NORMAL (restauración desde snapshot)
     */
    void initializeStateMachine();
    
    /**
     * @brief Actualiza el estado de la máquina de estados
     * 
     * LÓGICA DE ACTUALIZACIÓN:
     * - CLOSED si _closed es true
     * - EMPTY si la cola está vacía
     * - NORMAL si la cola tiene elementos
     * 
     * Solo realiza la transición si es posible y necesaria.
     */
    void updateState();
    
    /**
     * @brief Implementación común para operaciones de extracción
     * @tparam Operation Tipo del functor que realiza la extracción específica
     * @param operation Functor que ejecuta la extracción (back o front)
     * @param operation_name Nombre de la operación para logging
     * @return Elemento extraído
     * @throw EmptyQueueException si la cola está vacía
     * 
     * Este método centraliza la lógica común de:
     * - Sincronización thread-safe
     * - Verificación de cola vacía
     * - Logging de operaciones
     * - Actualización de estado
     * - Notificación de eventos
     */
    template<typename Operation>
    TType pop_impl(Operation operation, const std::string& operation_name);
};

// Incluir implementaciones de templates
#include "thread_safe_queue.tpp"

#endif // THREAD_SAFE_QUEUE_HPP