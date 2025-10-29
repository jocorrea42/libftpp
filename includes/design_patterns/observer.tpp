#ifndef OBSERVER_TPP
#define OBSERVER_TPP

#include "observer.hpp"

// ============ IMPLEMENTACIÓN PARA OBSERVER CON ARGUMENTO ============

/**
 * @brief Constructor por defecto para Observer con argumentos
 */
template<typename TEvent, typename TArg>
Observer<TEvent, TArg>::Observer()
{
    // El mapa se inicializa automáticamente vacío
}

/**
 * @brief Constructor de copia para Observer con argumentos
 * @param other Otro Observer del cual copiar todas las suscripciones
 */
template<typename TEvent, typename TArg>
Observer<TEvent, TArg>::Observer(const Observer& other) : subscribers_(other.subscribers_)
{
    // Copia profunda de todos los suscriptores
}

/**
 * @brief Operador de asignación para Observer con argumentos
 * @param other Otro Observer del cual copiar todas las suscripciones
 * @return Observer& Referencia a este Observer
 */
template<typename TEvent, typename TArg>
Observer<TEvent, TArg>& Observer<TEvent, TArg>::operator=(const Observer& other)
{
    // Verificar auto-asignación
    if (this != &other)
    {
        subscribers_ = other.subscribers_;
    }
    return *this;
}

/**
 * @brief Destructor para Observer con argumentos
 */
template<typename TEvent, typename TArg>
Observer<TEvent, TArg>::~Observer()
{
    // Los vectores y el mapa se destruyen automáticamente
    // Las funciones lambda se destruyen adecuadamente
}

/**
 * @brief Suscribir una función lambda a un evento específico (con argumentos)
 * @param event Evento al cual suscribirse
 * @param lambda Función a ejecutar cuando se notifique el evento
 * 
 * @note Si el evento no existe en el mapa, se crea automáticamente
 * @note La lambda se añade al final de la lista de suscriptores para ese evento
 */
template<typename TEvent, typename TArg>
void Observer<TEvent, TArg>::subscribe(const TEvent& event, const std::function<void(TArg)>& lambda)
{
    subscribers_[event].push_back(lambda);
}

/**
 * @brief Notificar a todos los suscriptores de un evento (con argumentos)
 * @param event Evento a notificar
 * @param arg Argumento a pasar a todos los suscriptores
 * 
 * @note Si el evento no tiene suscriptores, no se realiza ninguna acción
 * @note Las excepciones lanzadas por los suscriptores no son capturadas
 */
template<typename TEvent, typename TArg>
void Observer<TEvent, TArg>::notify(const TEvent& event, TArg arg)
{
    auto it = subscribers_.find(event);
    if (it != subscribers_.end())
    {
        // Ejecutar todos los suscriptores en el orden de suscripción
        for (const auto& subscriber : it->second)
        {
            subscriber(arg);
        }
    }
}

/**
 * @brief Desuscribir todos los listeners de un evento
 * @param event Evento del cual desuscribir todos los listeners
 * 
 * @note Si el evento no existe, no se realiza ninguna acción
 * @note Elimina completamente el evento del mapa de suscripciones
 */
template<typename TEvent, typename TArg>
void Observer<TEvent, TArg>::unsubscribe(const TEvent& event)
{
    subscribers_.erase(event);
}

/**
 * @brief Verificar si un evento tiene suscriptores
 * @param event Evento a verificar
 * @return true si el evento tiene al menos un suscriptor, false en caso contrario
 */
template<typename TEvent, typename TArg>
bool Observer<TEvent, TArg>::has_subscribers(const TEvent& event) const
{
    auto it = subscribers_.find(event);
    return it != subscribers_.end() && !it->second.empty();
}

/**
 * @brief Obtener número de suscriptores para un evento
 * @param event Evento a consultar
 * @return Número de suscriptores para el evento especificado
 */
template<typename TEvent, typename TArg>
size_t Observer<TEvent, TArg>::get_subscriber_count(const TEvent& event) const
{
    auto it = subscribers_.find(event);
    if (it != subscribers_.end())
    {
        return it->second.size();
    }
    return 0;
}

// ============ IMPLEMENTACIÓN PARA OBSERVER SIN ARGUMENTOS ============

/**
 * @brief Constructor por defecto para Observer sin argumentos
 */
template<typename TEvent>
Observer<TEvent, void>::Observer()
{
    // El mapa se inicializa automáticamente vacío
}

/**
 * @brief Constructor de copia para Observer sin argumentos
 * @param other Otro Observer del cual copiar todas las suscripciones
 */
template<typename TEvent>
Observer<TEvent, void>::Observer(const Observer& other) : subscribers_(other.subscribers_)
{
    // Copia profunda de todos los suscriptores
}

/**
 * @brief Operador de asignación para Observer sin argumentos
 * @param other Otro Observer del cual copiar todas las suscripciones
 * @return Observer& Referencia a este Observer
 */
template<typename TEvent>
Observer<TEvent, void>& Observer<TEvent, void>::operator=(const Observer& other)
{
    // Verificar auto-asignación
    if (this != &other)
    {
        subscribers_ = other.subscribers_;
    }
    return *this;
}

/**
 * @brief Destructor para Observer sin argumentos
 */
template<typename TEvent>
Observer<TEvent, void>::~Observer()
{
    // Los vectores y el mapa se destruyen automáticamente
}

/**
 * @brief Suscribir una función lambda a un evento específico (sin argumentos)
 * @param event Evento al cual suscribirse
 * @param lambda Función a ejecutar cuando se notifique el evento
 */
template<typename TEvent>
void Observer<TEvent, void>::subscribe(const TEvent& event, const std::function<void()>& lambda)
{
    subscribers_[event].push_back(lambda);
}

/**
 * @brief Notificar a todos los suscriptores de un evento (sin argumentos)
 * @param event Evento a notificar
 * 
 * @note Si el evento no tiene suscriptores, no se realiza ninguna acción
 * @note Las excepciones lanzadas por los suscriptores no son capturadas
 */
template<typename TEvent>
void Observer<TEvent, void>::notify(const TEvent& event)
{
    auto it = subscribers_.find(event);
    if (it != subscribers_.end())
    {
        // Ejecutar todos los suscriptores en el orden de suscripción
        for (const auto& subscriber : it->second)
        {
            subscriber();
        }
    }
}

/**
 * @brief Desuscribir todos los listeners de un evento
 * @param event Evento del cual desuscribir todos los listeners
 */
template<typename TEvent>
void Observer<TEvent, void>::unsubscribe(const TEvent& event)
{
    subscribers_.erase(event);
}

/**
 * @brief Verificar si un evento tiene suscriptores
 * @param event Evento a verificar
 * @return true si el evento tiene al menos un suscriptor, false en caso contrario
 */
template<typename TEvent>
bool Observer<TEvent, void>::has_subscribers(const TEvent& event) const
{
    auto it = subscribers_.find(event);
    return it != subscribers_.end() && !it->second.empty();
}

/**
 * @brief Obtener número de suscriptores para un evento
 * @param event Evento a consultar
 * @return Número de suscriptores para el evento especificado
 */
template<typename TEvent>
size_t Observer<TEvent, void>::get_subscriber_count(const TEvent& event) const
{
    auto it = subscribers_.find(event);
    if (it != subscribers_.end())
    {
        return it->second.size();
    }
    return 0;
}

#endif // OBSERVER_TPP