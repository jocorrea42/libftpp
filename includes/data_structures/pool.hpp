#ifndef POOL_HPP
#define POOL_HPP

#include <vector>
#include <stdexcept>
#include <memory>

/**
 * @class Pool
 * @brief Plantilla de clase para gestión de pool de objetos reutilizables.
 * 
 * Esta clase implementa un patrón de diseño Object Pool que permite reutilizar
 * objetos en lugar de crearlos y destruirlos constantemente, mejorando el rendimiento
 * en escenarios donde la creación de objetos es costosa.
 * 
 * Características principales:
 * - Pre-asignación de memoria para todos los objetos
 * - Reutilización de objetos sin overhead de asignación
 * - Gestión automática de la vida útil de los objetos
 * - Soporte para constructores con parámetros
 * - Seguridad de tipos mediante plantillas
 * - Semántica de movimiento para evitar copias
 * 
 * @tparam TType Tipo de objeto a gestionar en el pool. Debe ser default-constructible.
 */
template<typename TType>
class Pool
{
private:
    std::vector<std::unique_ptr<char[]>> memory_blocks_;  ///< Bloques de memoria cruda pre-asignados
    std::vector<TType*> objects_;                         ///< Punteros a objetos construidos
    std::vector<bool> available_;                         ///< Estado de disponibilidad de cada objeto
    size_t object_size_;                                  ///< Tamaño en bytes de cada objeto
    size_t capacity_;                                     ///< Capacidad actual del pool

public:
    // ============ CLASE INTERNA OBJECT ============
    
    /**
     * @class Object
     * @brief Clase wrapper que representa un objeto adquirido del pool.
     * 
     * Esta clase proporciona una interfaz segura para acceder a los objetos
     * del pool. Cuando un Object es destruido, el objeto subyacente se libera
     * automáticamente al pool para su reutilización.
     * 
     * Características:
     * - RAII: Liberación automática al pool al destruirse
     * - No copiable: Evita problemas de doble liberación
     * - Movible: Permite transferir ownership
     * - Operador ->: Acceso intuitivo a los métodos del objeto
     */
    class Object
    {
    private:
        Pool* owner_pool_;    ///< Referencia al pool propietario de este objeto
        TType* object_ptr_;   ///< Puntero al objeto gestionado
        size_t object_index_; ///< Índice del objeto en el pool

        /**
         * @brief Constructor privado - solo Pool puede crear Objects
         * @param pool Puntero al pool propietario
         * @param ptr Puntero al objeto gestionado
         * @param index Índice del objeto en el pool
         */
        Object(Pool* pool, TType* ptr, size_t index);

    public:
        // ============ DESTRUCTOR Y CONTROL DE COPIA/MOVIMIENTO ============
        
        /**
         * @brief Destructor - libera automáticamente el objeto al pool
         */
        ~Object();
        
        // Eliminar constructores de copia para evitar doble liberación
        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;
        
        /**
         * @brief Constructor de movimiento
         * @param other Otro Object a mover
         */
        Object(Object&& other) noexcept;
        
        /**
         * @brief Operador de asignación de movimiento
         * @param other Otro Object a mover
         * @return Referencia a este Object
         */
        Object& operator=(Object&& other) noexcept;
        
        // ============ OPERADORES Y MÉTODOS DE ACCESO ============
        
        /**
         * @brief Operador flecha para acceso a métodos del objeto subyacente
         * @return Puntero al objeto gestionado
         * @throw std::runtime_error si el objeto no es válido
         */
        TType* operator->() const;
        
        /**
         * @brief Obtener puntero crudo al objeto gestionado
         * @return Puntero al objeto o nullptr si no es válido
         */
        TType* get() const;
        
        /**
         * @brief Verificar si el Object es válido
         * @return true si el Object gestiona un objeto válido
         */
        bool is_valid() const;

        // Amigo para que Pool pueda acceder al constructor privado
        friend class Pool;
        TType& operator*() const {
        if (!object_ptr_) {
            throw std::runtime_error("Pool::Object: objeto no válido para dereferencia");
        }
        return *object_ptr_;
    }
    };

    // ============ CONSTRUCTOR Y DESTRUCTOR DEL POOL ============
    
    /**
     * @brief Constructor del pool
     * 
     * Inicializa un pool vacío con capacidad 0. La memoria se asignará
     * posteriormente mediante resize().
     */
    Pool();
    
    /**
     * @brief Destructor del pool
     * 
     * Destruye todos los objetos y libera toda la memoria asignada.
     */
    ~Pool();
    
    // ============ MÉTODOS PÚBLICOS DE GESTIÓN DEL POOL ============
    
    /**
     * @brief Redimensionar el pool - CONSTRUYE objetos inmediatamente
     * @param new_capacity Nueva capacidad del pool
     * @throw std::runtime_error si se intenta reducir la capacidad
     * 
     * Este método:
     * 1. Asigna nuevos bloques de memoria para la capacidad adicional
     * 2. Construye objetos usando el constructor por defecto
     * 3. Marca todos los nuevos objetos como disponibles
     * 
     * @note No se puede reducir la capacidad del pool una vez asignada.
     */
    void resize(const size_t& new_capacity);
    
    /**
     * @brief Adquirir objeto del pool - SIN argumentos (usa objetos pre-construidos)
     * @return Object que gestiona el objeto adquirido
     * @throw std::runtime_error si no hay objetos disponibles
     * 
     * Utiliza objetos previamente construidos con el constructor por defecto.
     * Más eficiente pero requiere que el objeto sea default-constructible.
     */
    Object acquire();
    
    /**
     * @brief Adquirir objeto del pool - CON argumentos (reconstruye el objeto)
     * @tparam TArgs Tipos de los argumentos del constructor
     * @param args Argumentos para construir el objeto
     * @return Object que gestiona el objeto adquirido
     * @throw std::runtime_error si no hay objetos disponibles
     * 
     * Reconstruye el objeto en la memoria pre-asignada usando los argumentos
     * proporcionados. Permite inicializar objetos con valores específicos.
     */
    template<typename... TArgs>
    Object acquire(TArgs&&... args);
    
    /**
     * @brief Obtener capacidad actual del pool
     * @return Número máximo de objetos que el pool puede contener
     */
    size_t get_capacity() const;
    
    /**
     * @brief Obtener número de objetos disponibles
     * @return Número de objetos actualmente disponibles para adquisición
     */
    size_t get_available_count() const;

private:
    // ============ MÉTODOS PRIVADOS ============
    
    /**
     * @brief Liberar objeto (llamado por Object al destruirse)
     * @param index Índice del objeto a liberar
     */
    void release(size_t index);
    
    /**
     * @brief Reconstruir objeto en una posición específica
     * @tparam TArgs Tipos de los argumentos del constructor
     * @param index Índice donde reconstruir el objeto
     * @param args Argumentos para el constructor
     * @throw std::runtime_error si el índice es inválido
     * 
     * Destruye el objeto existente y construye uno nuevo usando placement new
     * con los argumentos proporcionados.
     */
    template<typename... TArgs>
    void reconstruct_object(size_t index, TArgs&&... args);
    
    // Eliminar constructores de copia
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
};

// Incluir implementaciones de templates
#include "pool.tpp"

#endif // POOL_HPP