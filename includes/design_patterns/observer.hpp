#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <functional>
#include <vector>
#include <map>
#include <stdexcept>

/**
 * @class Observer
 * @brief Implementación del patrón Observer para notificación de eventos con soporte para argumentos.
 * 
 * Esta plantilla permite crear sistemas de publicación-suscripción donde los objetos pueden
 * suscribirse a eventos específicos y ser notificados cuando esos eventos ocurren.
 * 
 * Características principales:
 * - Soporte para eventos de cualquier tipo comparable (enum, string, int, etc.)
 * - Versiones con y sin argumentos para máxima flexibilidad
 * - Múltiples suscriptores por evento
 * - Gestión segura de memoria y suscripciones
 * - Operaciones de suscripción/desuscripción en tiempo constante amortizado
 * 
 * @tparam TEvent Tipo del evento (debe ser comparable, usualmente enum o string)
 * @tparam TArg Tipo del argumento pasado a los suscriptores (void para sin argumentos)
 * 
 * @example
 * // Con argumentos:
 * Observer<EventType, std::string> observer;
 * observer.subscribe(EventType::ERROR, [](const std::string& msg) { 
 *     std::cout << "Error: " << msg << std::endl; 
 * });
 * 
 * // Sin argumentos:
 * Observer<EventType> observer;
 * observer.subscribe(EventType::CLICK, []() { 
 *     std::cout << "Button clicked!" << std::endl; 
 * });
 */
template<typename TEvent, typename TArg = void>
class Observer
{
private:
    /// @brief Mapa que asocia cada evento con una lista de funciones suscriptoras
    std::map<TEvent, std::vector<std::function<void(TArg)>>> subscribers_;

public:
    // ============ CONSTRUCTORES Y DESTRUCTOR ============
    
    /**
     * @brief Constructor por defecto
     */
    Observer();
    
    /**
     * @brief Constructor de copia
     * @param other Otro Observer a copiar
     */
    Observer(const Observer& other);
    
    /**
     * @brief Operador de asignación
     * @param other Otro Observer a copiar
     * @return Referencia a este Observer
     */
    Observer& operator=(const Observer& other);
    
    /**
     * @brief Destructor
     */
    ~Observer();

    // ============ MÉTODOS PÚBLICOS PRINCIPALES ============
    
    /**
     * @brief Suscribir una función lambda a un evento específico
     * @param event Evento al cual suscribirse
     * @param lambda Función a ejecutar cuando se notifique el evento
     * 
     * @note Las lambdas se ejecutarán en el orden en que fueron suscritas
     * @note Permite múltiples suscriptores para el mismo evento
     */
    void subscribe(const TEvent& event, const std::function<void(TArg)>& lambda);
    
    /**
     * @brief Notificar a todos los suscriptores de un evento
     * @param event Evento a notificar
     * @param arg Argumento a pasar a los suscriptores
     * 
     * Ejecuta todas las funciones suscritas al evento en el orden de suscripción.
     * Si no hay suscriptores para el evento, no realiza ninguna acción.
     */
    void notify(const TEvent& event, TArg arg);
    
    /**
     * @brief Desuscribir todos los listeners de un evento
     * @param event Evento del cual desuscribir todos los listeners
     */
    void unsubscribe(const TEvent& event);
    
    /**
     * @brief Verificar si un evento tiene suscriptores
     * @param event Evento a verificar
     * @return true si el evento tiene al menos un suscriptor, false en caso contrario
     */
    bool has_subscribers(const TEvent& event) const;
    
    /**
     * @brief Obtener número de suscriptores para un evento
     * @param event Evento a consultar
     * @return Número de suscriptores para el evento especificado
     */
    size_t get_subscriber_count(const TEvent& event) const;
};

// ============ ESPECIALIZACIÓN PARA SIN ARGUMENTOS ============

/**
 * @class Observer
 * @brief Especialización para Observer sin argumentos (backward compatibility)
 * 
 * Esta especialización mantiene compatibilidad con el uso original del patrón Observer
 * donde los eventos no requieren pasar argumentos a los suscriptores.
 * 
 * @tparam TEvent Tipo del evento (debe ser comparable)
 */
template<typename TEvent>
class Observer<TEvent, void>
{
private:
    /// @brief Mapa de eventos a listas de suscriptores sin argumentos
    std::map<TEvent, std::vector<std::function<void()>>> subscribers_;

public:
    // ============ CONSTRUCTORES Y DESTRUCTOR ============
    
    /**
     * @brief Constructor por defecto
     */
    Observer();
    
    /**
     * @brief Constructor de copia
     * @param other Otro Observer a copiar
     */
    Observer(const Observer& other);
    
    /**
     * @brief Operador de asignación
     * @param other Otro Observer a copiar
     * @return Referencia a este Observer
     */
    Observer& operator=(const Observer& other);
    
    /**
     * @brief Destructor
     */
    ~Observer();

    // ============ MÉTODOS PÚBLICOS PRINCIPALES ============
    
    /**
     * @brief Suscribir una función lambda a un evento específico (sin argumentos)
     * @param event Evento al cual suscribirse
     * @param lambda Función a ejecutar cuando se notifique el evento
     */
    void subscribe(const TEvent& event, const std::function<void()>& lambda);
    
    /**
     * @brief Notificar a todos los suscriptores de un evento (sin argumentos)
     * @param event Evento a notificar
     */
    void notify(const TEvent& event);
    
    /**
     * @brief Desuscribir todos los listeners de un evento
     * @param event Evento del cual desuscribir todos los listeners
     */
    void unsubscribe(const TEvent& event);
    
    /**
     * @brief Verificar si un evento tiene suscriptores
     * @param event Evento a verificar
     * @return true si el evento tiene al menos un suscriptor, false en caso contrario
     */
    bool has_subscribers(const TEvent& event) const;
    
    /**
     * @brief Obtener número de suscriptores para un evento
     * @param event Evento a consultar
     * @return Número de suscriptores para el evento especificado
     */
    size_t get_subscriber_count(const TEvent& event) const;
};

// Incluir implementaciones de templates
#include "observer.tpp"

#endif // OBSERVER_HPP