#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <stdexcept>

/**
 * @class Singleton
 * @brief Implementación del patrón Singleton para garantizar una única instancia por tipo.
 * 
 * Esta plantilla asegura que una clase de tipo TType tenga solo una instancia en todo el programa
 * y proporciona un punto de acceso global a dicha instancia.
 * 
 * Características principales:
 * - Instancia única por tipo template
 * - Creación perezosa (lazy initialization)
 * - Soporte para constructores con parámetros
 * - Gestión segura del ciclo de vida
 * - Prevención de copia y asignación
 * - Verificación de instanciación múltiple
 * 
 * @tparam TType Tipo de la clase que se desea hacer Singleton
 * 
 * @note La clase TType debe declarar a Singleton<TType> como friend para permitir
 *       el acceso a su constructor, o utilizar SingletonAccess.
 * 
 * @example
 * class MyClass {
 *     friend class Singleton<MyClass>;  // O usar SingletonAccess
 * private:
 *     MyClass(int value) { ... }        // Constructor privado
 * };
 * 
 * // Uso:
 * Singleton<MyClass>::instantiate(42);  // Crear instancia con argumentos
 * MyClass* instance = Singleton<MyClass>::instance();  // Obtener instancia
 * Singleton<MyClass>::destroy();        // Destruir instancia
 */
template<typename TType>
class Singleton
{
private:
    static TType* instance_;  ///< Puntero a la instancia única

public:
    // ============ MÉTODOS PÚBLICOS ESTÁTICOS ============
    
    /**
     * @brief Obtener la instancia única de la clase
     * @return TType* Puntero a la instancia única
     * @throw std::runtime_error si la instancia no ha sido creada
     * 
     * Este método proporciona acceso global a la instancia única.
     * Debe llamarse después de haber creado la instancia con instantiate().
     */
    static TType* instance();
    
    /**
     * @brief Crear la instancia única con argumentos
     * @tparam TArgs Tipos de los argumentos del constructor
     * @param args Argumentos para pasar al constructor de TType
     * @throw std::runtime_error si la instancia ya ha sido creada
     * 
     * Este método debe llamarse exactamente una vez antes de usar instance().
     * Si la instancia ya existe, lanza una excepción.
     */
    template<typename... TArgs>
    static void instantiate(TArgs&&... args);
    
    /**
     * @brief Destruir la instancia única
     * 
     * Libera la instancia única y establece el puntero a nullptr.
     * Si no existe instancia, no realiza ninguna acción.
     */
    static void destroy();
    
    /**
     * @brief Verificar si la instancia existe
     * @return true si la instancia ha sido creada, false en caso contrario
     */
    static bool is_instantiated();

private:
    // ============ PREVENCIÓN DE INSTANCIACIÓN ============
    
    /**
     * @brief Constructor eliminado - Clase de solo métodos estáticos
     */
    Singleton() = delete;
    
    /**
     * @brief Constructor de copia eliminado
     */
    Singleton(const Singleton&) = delete;
    
    /**
     * @brief Operador de asignación eliminado
     */
    Singleton& operator=(const Singleton&) = delete;
    
    // ============ AMISTAD PARA ACCESO AL CONSTRUCTOR ============
    
    /**
     * @brief Clase amiga para permitir acceso al constructor de TType
     * 
     * Esta amistad permite a SingletonAccess crear instancias de TType
     * incluso si el constructor es privado.
     */
    template<typename> friend class SingletonAccess;
};

// ============ CLASE AUXILIAR SINGLETONACCESS ============

/**
 * @class SingletonAccess
 * @brief Clase auxiliar para acceder al constructor de clases con constructores privados
 * 
 * Esta clase template proporciona métodos estáticos para crear y destruir instancias
 * de TType, permitiendo que Singleton funcione incluso con clases que tienen
 * constructores privados.
 * 
 * @tparam TType Tipo de la clase a instanciar
 */
template<typename TType>
class SingletonAccess
{
public:
    /**
     * @brief Crear una nueva instancia de TType
     * @tparam TArgs Tipos de los argumentos del constructor
     * @param args Argumentos para el constructor
     * @return TType* Puntero a la nueva instancia
     */
    template<typename... TArgs>
    static TType* create_instance(TArgs&&... args)
    {
        return new TType(std::forward<TArgs>(args)...);
    }
    
    /**
     * @brief Destruir una instancia de TType
     * @param instance Puntero a la instancia a destruir
     */
    static void destroy_instance(TType* instance)
    {
        delete instance;
    }
};

// Incluir implementaciones de templates
#include "singleton.tpp"

#endif // SINGLETON_HPP