#ifndef SINGLETON_TPP
#define SINGLETON_TPP

#include "singleton.hpp"

// ============ INICIALIZACIÓN DE VARIABLES ESTÁTICAS ============

/**
 * @brief Inicialización del puntero a instancia única
 * 
 * Esta definición inicializa el puntero estático instance_ a nullptr
 * para cada especialización de Singleton<TType>.
 */
template<typename TType>
TType* Singleton<TType>::instance_ = nullptr;

// ============ IMPLEMENTACIÓN DE MÉTODOS PÚBLICOS ============

/**
 * @brief Obtener la instancia única de la clase
 * 
 * @return TType* Puntero a la instancia única
 * @throw std::runtime_error si la instancia no ha sido creada
 * 
 * Este método implementa el acceso global a la instancia única.
 * Si la instancia no ha sido creada previamente (instance_ es nullptr),
 * lanza una excepción para evitar comportamientos indefinidos.
 */
template<typename TType>
TType* Singleton<TType>::instance()
{
    if (!instance_)
    {
        throw std::runtime_error("Instance not yet created");
    }
    return instance_;
}

/**
 * @brief Crear la instancia única con argumentos
 * 
 * @tparam TArgs Tipos de los argumentos del constructor
 * @param args Argumentos para pasar al constructor de TType
 * @throw std::runtime_error si la instancia ya ha sido creada
 * 
 * Este método garantiza que solo se cree una instancia de TType.
 * Utiliza perfect forwarding para pasar los argumentos al constructor
 * y delega la creación en SingletonAccess para manejar constructores privados.
 */
template<typename TType>
template<typename... TArgs>
void Singleton<TType>::instantiate(TArgs&&... args)
{
    // Verificar que no exista una instancia previa
    if (instance_)
    {
        throw std::runtime_error("Instance already created");
    }
    
    // Crear la instancia única usando SingletonAccess
    instance_ = SingletonAccess<TType>::create_instance(std::forward<TArgs>(args)...);
}

/**
 * @brief Destruir la instancia única
 * 
 * Este método libera la instancia única si existe y establece el puntero a nullptr.
 * Es seguro llamar a este método incluso si la instancia no existe.
 */
template<typename TType>
void Singleton<TType>::destroy()
{
    if (instance_)
    {
        // Destruir la instancia usando SingletonAccess
        SingletonAccess<TType>::destroy_instance(instance_);
        instance_ = nullptr;
    }
}

/**
 * @brief Verificar si la instancia existe
 * 
 * @return true si la instancia ha sido creada, false en caso contrario
 * 
 * Este método permite verificar de forma segura si la instancia única
 * ha sido creada antes de intentar acceder a ella.
 */
template<typename TType>
bool Singleton<TType>::is_instantiated()
{
    return instance_ != nullptr;
}

#endif // SINGLETON_TPP