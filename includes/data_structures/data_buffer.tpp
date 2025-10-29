#ifndef DATA_BUFFER_TPP
#define DATA_BUFFER_TPP

#include "data_buffer.hpp"

// ============ IMPLEMENTACIONES DE TEMPLATES ============

/**
 * @brief Operador de inserción (serialización) para tipos básicos.
 * 
 * @tparam T Tipo del dato a serializar. Debe ser un tipo que pueda copiarse
 *           directamente con memcpy (tipos POD - Plain Old Data).
 * @param data Dato a serializar en el buffer.
 * @return DataBuffer& Referencia a este buffer para encadenamiento.
 * 
 * @note Esta implementación es adecuada para tipos básicos como:
 *       - Tipos enteros (int, long, short, etc.)
 *       - Tipos de punto flotante (float, double)
 *       - Tipos booleanos (bool)
 *       - Estructuras simples sin punteros ni manejo dinámico de memoria
 * 
 * @warning No usar con tipos complejos que requieran constructores de copia
 *          o que contengan punteros a memoria dinámica.
 */
template<typename T>
DataBuffer& DataBuffer::operator<<(const T& data)
{
    // Convertir el dato a un array de bytes usando reinterpret_cast
    const char* bytes = reinterpret_cast<const char*>(&data);
    
    // Escribir los bytes en el buffer
    write_bytes(bytes, sizeof(T));
    
    return *this;
}

/**
 * @brief Operador de extracción (deserialización) para tipos básicos.
 * 
 * @tparam T Tipo del dato a deserializar. Debe ser el mismo tipo usado
 *           durante la serialización.
 * @param data Variable donde se almacenará el dato deserializado.
 * @return DataBuffer& Referencia a este buffer para encadenamiento.
 * 
 * @throw std::runtime_error si no hay suficientes bytes en el buffer
 *        para completar la lectura.
 * 
 * @note La deserialización debe realizarse en el mismo orden que la serialización.
 *       Leer un tipo diferente al escrito resultará en datos corruptos.
 */
template<typename T>
DataBuffer& DataBuffer::operator>>(T& data)
{
    // Convertir la variable destino a un array de bytes para escritura
    char* bytes = reinterpret_cast<char*>(&data);
    
    // Leer los bytes desde el buffer
    read_bytes(bytes, sizeof(T));
    
    return *this;
}

#endif // DATA_BUFFER_TPP