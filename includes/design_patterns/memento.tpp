#ifndef MEMENTO_TPP
#define MEMENTO_TPP

#include "memento.hpp"

// ============ IMPLEMENTACIONES DE TEMPLATES PARA SNAPSHOT ============

/**
 * @brief Operador de inserción para serializar datos en el Snapshot
 * 
 * @tparam T Tipo del dato a serializar
 * @param data Dato a serializar
 * @return Memento::Snapshot& Referencia a este Snapshot para encadenamiento
 * 
 * Este método template permite serializar cualquier tipo de dato compatible
 * con DataBuffer. Delega en el operador << de DataBuffer.
 */
template<typename T>
Memento::Snapshot& Memento::Snapshot::operator<<(const T& data)
{
    buffer_ << data;
    return *this;
}

/**
 * @brief Operador de extracción para deserializar datos del Snapshot
 * 
 * @tparam T Tipo del dato a deserializar
 * @param data Variable donde almacenar el dato deserializado
 * @return Memento::Snapshot& Referencia a este Snapshot para encadenamiento
 * 
 * Este método template permite deserializar cualquier tipo de dato compatible
 * con DataBuffer. Delega en el operador >> de DataBuffer.
 * 
 * @throw std::runtime_error si no hay suficientes datos o el tipo no coincide
 */
template<typename T>
Memento::Snapshot& Memento::Snapshot::operator>>(T& data)
{
    buffer_ >> data;
    return *this;
}

#endif // MEMENTO_TPP