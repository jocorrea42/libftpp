#ifndef POOL_TPP
#define POOL_TPP

#include "pool.hpp"
#include <new>       // Para placement new
#include <utility>   // Para std::forward

// ============ IMPLEMENTACIÓN DE LA CLASE INTERNA OBJECT ============

/**
 * @brief Constructor de Object (privado, solo Pool puede crear)
 * 
 * @param pool Puntero al pool propietario
 * @param ptr Puntero al objeto gestionado
 * @param index Índice del objeto en el pool
 * 
 * Este constructor es privado para garantizar que solo Pool pueda crear
 * instancias de Object, manteniendo el control sobre el ciclo de vida.
 */
template<typename TType>
Pool<TType>::Object::Object(Pool* pool, TType* ptr, size_t index) 
    : owner_pool_(pool), object_ptr_(ptr), object_index_(index)
{
}

/**
 * @brief Destructor de Object - libera automáticamente al pool
 * 
 * Cuando un Object es destruido (ya sea por scope o explícitamente),
 * libera el objeto subyacente al pool para que pueda ser reutilizado.
 * Esto implementa el patrón RAII (Resource Acquisition Is Initialization).
 */
template<typename TType>
Pool<TType>::Object::~Object()
{
    // Solo liberar si el objeto y el pool son válidos
    if (object_ptr_ && owner_pool_)
    {
        owner_pool_->release(object_index_);
    }
}

/**
 * @brief Constructor de movimiento
 * 
 * @param other Otro Object a mover
 * 
 * Transfiere la propiedad del objeto desde 'other' hacia este Object.
 * Después del movimiento, 'other' queda en estado inválido (nullptr).
 */
template<typename TType>
Pool<TType>::Object::Object(Object&& other) noexcept
    : owner_pool_(other.owner_pool_)
    , object_ptr_(other.object_ptr_)
    , object_index_(other.object_index_)
{
    // Invalidar el objeto fuente para evitar doble liberación
    other.object_ptr_ = nullptr;
    other.owner_pool_ = nullptr;
}

/**
 * @brief Operador de asignación de movimiento
 * 
 * @param other Otro Object a mover
 * @return Referencia a este Object
 * 
 * Libera el objeto actual (si existe) y luego transfiere la propiedad
 * desde 'other'. Maneja correctamente la auto-asignación.
 */
template<typename TType>
typename Pool<TType>::Object& Pool<TType>::Object::operator=(Object&& other) noexcept
{
    // Verificar auto-asignación
    if (this != &other)
    {
        // Liberar el objeto actual si es válido
        if (object_ptr_ && owner_pool_)
        {
            owner_pool_->release(object_index_);
        }
        
        // Transferir propiedad desde 'other'
        owner_pool_ = other.owner_pool_;
        object_ptr_ = other.object_ptr_;
        object_index_ = other.object_index_;
        
        // Invalidar el objeto fuente
        other.object_ptr_ = nullptr;
        other.owner_pool_ = nullptr;
    }
    return *this;
}

/**
 * @brief Operador flecha para acceso a métodos del objeto
 * 
 * @return Puntero al objeto gestionado
 * @throw std::runtime_error si el objeto no es válido
 * 
 * Permite acceder a los métodos del objeto subyacente de forma intuitiva:
 * @code
 * Pool<MiClase>::Object obj = pool.acquire();
 * obj->miMetodo();  // Llama a MiClase::miMetodo()
 * @endcode
 */
template<typename TType>
TType* Pool<TType>::Object::operator->() const
{
    if (!object_ptr_)
    {
        throw std::runtime_error("Pool::Object: objeto no válido");
    }
    return object_ptr_;
}

/**
 * @brief Obtener puntero crudo al objeto gestionado
 * 
 * @return Puntero al objeto o nullptr si no es válido
 * 
 * Útil para cuando se necesita el puntero directo sin la semántica
 * de operador flecha.
 */
template<typename TType>
TType* Pool<TType>::Object::get() const
{
    return object_ptr_;
}

/**
 * @brief Verificar si el Object es válido
 * 
 * @return true si el Object gestiona un objeto válido
 * 
 * Un Object puede ser inválido si:
 * - Fue movido a otro Object
 * - Fue construido por defecto (no por Pool)
 * - El pool fue destruido
 */
template<typename TType>
bool Pool<TType>::Object::is_valid() const
{
    return object_ptr_ != nullptr;
}

// ============ IMPLEMENTACIÓN DE LA CLASE POOL ============

/**
 * @brief Constructor de Pool
 * 
 * Inicializa un pool vacío con:
 * - Capacidad 0
 * - Tamaño del objeto calculado con sizeof
 * - Vectores vacíos
 * 
 * La memoria se asignará posteriormente mediante resize().
 */
template<typename TType>
Pool<TType>::Pool() 
    : object_size_(sizeof(TType)), capacity_(0)
{
}

/**
 * @brief Destructor de Pool
 * 
 * Realiza limpieza completa:
 * 1. Destruye todos los objetos usando destructor explícito
 * 2. Libera automáticamente la memoria gracias a std::unique_ptr
 * 3. Todos los vectores se destruyen automáticamente
 * 
 * @note Los Objects que aún estén activos quedarán inválidos
 */
template<typename TType>
Pool<TType>::~Pool()
{
    // Destruir todos los objetos explícitamente
    for (size_t i = 0; i < capacity_; ++i)
    {
        if (objects_[i] != nullptr)
        {
            objects_[i]->~TType();
        }
    }
}

/**
 * @brief Redimensionar pool - CONSTRUYE objetos inmediatamente
 * 
 * @param new_capacity Nueva capacidad del pool
 * @throw std::runtime_error si se intenta reducir la capacidad
 * 
 * Este método es fundamental para inicializar el pool. Asigna memoria
 * y construye objetos usando el constructor por defecto de TType.
 * 
 * Proceso:
 * 1. Verifica que new_capacity >= capacidad actual
 * 2. Redimensiona todos los vectores internos
 * 3. Para cada nueva posición:
 *    - Asigna bloque de memoria
 *    - Construye objeto con placement new
 *    - Marca como disponible
 */
template<typename TType>
void Pool<TType>::resize(const size_t& new_capacity)
{
    // No permitir reducir capacidad (complicaría la gestión)
    if (new_capacity < capacity_)
    {
        throw std::runtime_error("Pool: no se puede reducir la capacidad");
    }
    
    // Redimensionar vectores internos
    memory_blocks_.resize(new_capacity);
    objects_.resize(new_capacity, nullptr);
    available_.resize(new_capacity, true);
    
    // Asignar nueva memoria y construir objetos para los nuevos bloques
    for (size_t i = capacity_; i < new_capacity; ++i)
    {
        // Asignar memoria cruda para el objeto
        memory_blocks_[i] = std::unique_ptr<char[]>(new char[object_size_]);
        
        // Construir objeto en la memoria asignada usando constructor por defecto
        objects_[i] = new (memory_blocks_[i].get()) TType();
    }
    
    // Actualizar capacidad
    capacity_ = new_capacity;
}

/**
 * @brief Adquirir objeto del pool - SIN argumentos
 * 
 * @return Object que gestiona el objeto adquirido
 * @throw std::runtime_error si no hay objetos disponibles
 * 
 * Busca el primer objeto disponible en el pool y lo marca como no disponible.
 * Utiliza objetos pre-construidos, por lo que es más eficiente pero requiere
 * que TType sea default-constructible.
 */
template<typename TType>
typename Pool<TType>::Object Pool<TType>::acquire()
{
    // Buscar primer objeto disponible
    for (size_t i = 0; i < capacity_; ++i)
    {
        if (available_[i])
        {
            // Marcar como no disponible y retornar Object
            available_[i] = false;
            return Object(this, objects_[i], i);
        }
    }
    throw std::runtime_error("Pool: no hay objetos disponibles");
}

/**
 * @brief Adquirir objeto del pool - CON argumentos
 * 
 * @tparam TArgs Tipos de los argumentos del constructor
 * @param args Argumentos para construir el objeto
 * @return Object que gestiona el objeto adquirido
 * @throw std::runtime_error si no hay objetos disponibles
 * 
 * Similar a acquire() pero reconstruye el objeto con los argumentos proporcionados.
 * Útil cuando se necesitan objetos con estado inicial específico.
 */
template<typename TType>
template<typename... TArgs>
typename Pool<TType>::Object Pool<TType>::acquire(TArgs&&... args)
{
    // Buscar primer objeto disponible
    for (size_t i = 0; i < capacity_; ++i)
    {
        if (available_[i])
        {
            // Marcar como no disponible
            available_[i] = false;
            
            // Reconstruir objeto con los argumentos proporcionados
            reconstruct_object(i, std::forward<TArgs>(args)...);
            
            // Retornar Object que gestiona el objeto reconstruido
            return Object(this, objects_[i], i);
        }
    }
    throw std::runtime_error("Pool: no hay objetos disponibles");
}

/**
 * @brief Reconstruir objeto en una posición específica
 * 
 * @tparam TArgs Tipos de los argumentos del constructor
 * @param index Índice donde reconstruir el objeto
 * @param args Argumentos para el constructor
 * @throw std::runtime_error si el índice es inválido
 * 
 * Este método permite "resetear" un objeto con nuevos valores usando
 * el patrón destructor + placement new.
 */
template<typename TType>
template<typename... TArgs>
void Pool<TType>::reconstruct_object(size_t index, TArgs&&... args)
{
    // Verificar que el índice sea válido
    if (index >= capacity_)
    {
        throw std::runtime_error("Pool: índice inválido para reconstrucción");
    }
    
    // Destruir objeto existente si hay uno
    if (objects_[index] != nullptr)
    {
        objects_[index]->~TType();
    }
    
    // Construir nuevo objeto con los argumentos usando placement new
    objects_[index] = new (memory_blocks_[index].get()) TType(std::forward<TArgs>(args)...);
}

/**
 * @brief Obtener capacidad actual del pool
 * 
 * @return Número máximo de objetos que el pool puede contener
 */
template<typename TType>
size_t Pool<TType>::get_capacity() const
{
    return capacity_;
}

/**
 * @brief Obtener número de objetos disponibles
 * 
 * @return Número de objetos actualmente disponibles para adquisición
 * 
 * Recorre el vector available_ contando los elementos con valor true.
 */
template<typename TType>
size_t Pool<TType>::get_available_count() const
{
    size_t count = 0;
    for (bool avail : available_)
    {
        if (avail) count++;
    }
    return count;
}

/**
 * @brief Liberar objeto (método privado)
 * 
 * @param index Índice del objeto a liberar
 * 
 * Este método es llamado por el destructor de Object cuando un objeto
 * es liberado al pool. Simplemente marca la posición como disponible.
 * 
 * @note No destruye el objeto - eso se hace durante la reconstrucción
 */
template<typename TType>
void Pool<TType>::release(size_t index)
{
    if (index < available_.size())
    {
        available_[index] = true;
    }
}

#endif // POOL_TPP