#include "data_structures/data_buffer.hpp"
#include <cstring>  // Para std::memcpy

// ============ IMPLEMENTACIONES DE MÉTODOS ============

/**
 * @brief Constructor por defecto.
 * 
 * Inicializa un buffer vacío con las posiciones de lectura y escritura en 0.
 * El vector interno se crea vacío y se redimensionará según sea necesario.
 */
DataBuffer::DataBuffer() : read_pos(0), write_pos(0) {}

/**
 * @brief Constructor de copia.
 * 
 * @param other Buffer del cual copiar todo el contenido y estado actual.
 * 
 * Crea una copia independiente del buffer original, incluyendo:
 * - Todos los datos almacenados
 * - Las posiciones actuales de lectura y escritura
 */
DataBuffer::DataBuffer(const DataBuffer& other) 
    : buffer(other.buffer), read_pos(other.read_pos), write_pos(other.write_pos) {}

/**
 * @brief Operador de asignación.
 * 
 * @param other Buffer del cual copiar el contenido.
 * @return DataBuffer& Referencia a este objeto para encadenamiento.
 * 
 * Realiza una copia profunda de todos los datos y estado del buffer original.
 * Si other es el mismo objeto (auto-asignación), no realiza ninguna operación.
 */
DataBuffer& DataBuffer::operator=(const DataBuffer& other) {
    // Verificar auto-asignación
    if (this != &other) {
        buffer = other.buffer;
        read_pos = other.read_pos;
        write_pos = other.write_pos;
    }
    return *this;
}

/**
 * @brief Destructor.
 * 
 * Libera automáticamente todos los recursos gracias al uso de std::vector.
 * No se requiere limpieza manual de memoria.
 */
DataBuffer::~DataBuffer() {}

// ============ MÉTODOS PÚBLICOS DE MANEJO DEL BUFFER ============

/**
 * @brief Añade una cadena de texto al buffer.
 * 
 * @param str Cadena a añadir al final del buffer.
 * 
 * Este método es útil cuando se quiere usar el DataBuffer como un simple
 * contenedor de texto, sin el formato de serialización con longitud.
 */
void DataBuffer::append(const std::string& str) {
    write_bytes(str.data(), str.size());
}

/**
 * @brief Convierte el contenido del buffer a una cadena de texto.
 * 
 * @return std::string con todo el contenido escrito en el buffer.
 * 
 * @note Solo incluye los datos hasta la posición de escritura actual.
 * Los datos más allá de write_pos (si los hay) no se incluyen.
 */
std::string DataBuffer::str() const {
    return std::string(buffer.begin(), buffer.begin() + write_pos);
}

/**
 * @brief Limpia completamente el buffer.
 * 
 * Reinicia el buffer a su estado inicial:
 * - Vacía el vector interno
 * - Establece read_pos y write_pos a 0
 * - Libera cualquier memoria reservada
 * 
 * El buffer queda listo para ser reutilizado desde cero.
 */
void DataBuffer::clear() {
    buffer.clear();
    read_pos = 0;
    write_pos = 0;
}

/**
 * @brief Verifica si el buffer está vacío.
 * 
 * @return true si no hay datos escritos (write_pos == 0), false en caso contrario.
 * 
 * @note Un buffer puede tener datos pero posición de lectura al final,
 * lo que significa que ya se han leído todos los datos disponibles.
 */
bool DataBuffer::empty() const {
    return write_pos == 0;
}

/**
 * @brief Obtiene el tamaño actual de los datos escritos.
 * 
 * @return Número de bytes actualmente almacenados en el buffer.
 * 
 * @note Este tamaño representa la cantidad total de datos escritos,
 * no necesariamente la cantidad de datos disponibles para lectura
 * (que sería write_pos - read_pos).
 */
size_t DataBuffer::size() const {
    return write_pos;
}

// ============ MÉTODOS PRIVADOS DE BAJO NIVEL ============

/**
 * @brief Garantiza que el buffer tenga capacidad suficiente.
 * 
 * @param needed Número mínimo de bytes necesarios.
 * 
 * Si el tamaño actual del vector es menor que la capacidad necesaria,
 * redimensiona el vector para que tenga al menos el tamaño needed.
 * No afecta a las posiciones de lectura/escritura.
 */
void DataBuffer::ensure_capacity(size_t needed) {
    if (buffer.size() < needed) {
        buffer.resize(needed);
    }
}

/**
 * @brief Escribe un bloque de bytes en el buffer.
 * 
 * @param data Puntero al inicio de los datos a escribir.
 * @param size Número de bytes a escribir.
 * 
 * Este método:
 * 1. Garantiza que haya capacidad suficiente
 * 2. Copia los datos al buffer usando memcpy
 * 3. Actualiza la posición de escritura
 * 
 * @note Es el método fundamental para todas las operaciones de escritura.
 */
void DataBuffer::write_bytes(const char* data, size_t size) {
    // Asegurar que hay espacio suficiente
    ensure_capacity(write_pos + size);
    
    // Copiar los datos al buffer
    std::memcpy(buffer.data() + write_pos, data, size);
    
    // Actualizar posición de escritura
    write_pos += size;
}

/**
 * @brief Lee un bloque de bytes del buffer.
 * 
 * @param data Puntero donde almacenar los bytes leídos.
 * @param size Número de bytes a leer.
 * 
 * @throw std::runtime_error si se intenta leer más allá del final del buffer.
 * 
 * Este método:
 * 1. Verifica que haya suficientes datos disponibles
 * 2. Copia los datos desde el buffer usando memcpy
 * 3. Actualiza la posición de lectura
 * 
 * @note Es el método fundamental para todas las operaciones de lectura.
 */
void DataBuffer::read_bytes(char* data, size_t size) {
    // Verificar que hay suficientes datos para leer
    if (read_pos + size > write_pos) {
        throw std::runtime_error("DataBuffer: lectura fuera de límites");
    }
    
    // Copiar los datos desde el buffer
    std::memcpy(data, buffer.data() + read_pos, size);
    
    // Actualizar posición de lectura
    read_pos += size;
}

// ============ ESPECIALIZACIONES PARA std::string ============

/**
 * @brief Operador de inserción especializado para cadenas.
 * 
 * @param data Cadena a serializar en el buffer.
 * @return DataBuffer& Referencia a este buffer para encadenamiento.
 * 
 * Las cadenas se serializan en un formato que incluye:
 * 1. La longitud de la cadena (size_t)
 * 2. El contenido de la cadena (caracteres)
 * 
 * Esto permite reconstruir exactamente la cadena original durante la deserialización,
 * incluso si contiene caracteres nulos.
 */
DataBuffer& DataBuffer::operator<<(const std::string& data) {
    // Primero serializar la longitud de la cadena
    size_t len = data.size();
    *this << len;
    
    // Luego serializar el contenido de la cadena
    write_bytes(data.data(), len);
    
    return *this;
}

/**
 * @brief Operador de extracción especializado para cadenas.
 * 
 * @param data Cadena donde se almacenará el resultado.
 * @return DataBuffer& Referencia a este buffer para encadenamiento.
 * 
 * @throw std::runtime_error si no hay suficientes datos para leer la cadena completa.
 * 
 * La deserialización sigue el mismo formato que la serialización:
 * 1. Lee la longitud de la cadena
 * 2. Lee el número correspondiente de caracteres
 * 3. Construye la cadena resultante
 */
DataBuffer& DataBuffer::operator>>(std::string& data) {
    // Primero deserializar la longitud de la cadena
    size_t len;
    *this >> len;
    
    // Redimensionar la cadena destino para acomodar los datos
    data.resize(len);
    
    // Deserializar el contenido de la cadena
    read_bytes(&data[0], len);
    
    return *this;
}