#include "design_patterns/memento.hpp"

// ============ IMPLEMENTACIÓN DE LA CLASE SNAPSHOT ============

/**
 * @brief Constructor por defecto de Snapshot
 * 
 * Inicializa un Snapshot vacío con un DataBuffer interno listo para usar.
 */
Memento::Snapshot::Snapshot() : buffer_()
{
}

/**
 * @brief Constructor de copia de Snapshot
 * 
 * @param other Otro Snapshot del cual copiar el contenido
 * 
 * Crea una copia independiente del Snapshot original, incluyendo
 * todos los datos serializados en el buffer interno.
 */
Memento::Snapshot::Snapshot(const Snapshot& other) : buffer_(other.buffer_)
{
}

/**
 * @brief Operador de asignación de Snapshot
 * 
 * @param other Otro Snapshot del cual copiar el contenido
 * @return Memento::Snapshot& Referencia a este Snapshot
 * 
 * Realiza una copia profunda del contenido del Snapshot original.
 * Maneja correctamente la auto-asignación.
 */
Memento::Snapshot& Memento::Snapshot::operator=(const Snapshot& other)
{
    // Verificar auto-asignación
    if (this != &other)
    {
        buffer_ = other.buffer_;
    }
    return *this;
}

/**
 * @brief Destructor de Snapshot
 * 
 * Libera automáticamente todos los recursos gracias al uso de DataBuffer.
 * No se requiere limpieza manual.
 */
Memento::Snapshot::~Snapshot()
{
}

/**
 * @brief Guardar una cadena en el snapshot
 * 
 * @param str Cadena a guardar
 * 
 * Utiliza el operador << de DataBuffer para serializar la cadena
 * en el formato adecuado (longitud + contenido).
 */
void Memento::Snapshot::save_string(const std::string& str)
{
    buffer_ << str;
}

/**
 * @brief Cargar una cadena desde el snapshot
 * 
 * @param str Cadena donde almacenar el resultado
 * 
 * Utiliza el operador >> de DataBuffer para deserializar la cadena
 * previamente guardada.
 * 
 * @throw std::runtime_error si no hay una cadena válida para cargar
 */
void Memento::Snapshot::load_string(std::string& str)
{
    buffer_ >> str;
}

/**
 * @brief Guardar un entero en el snapshot
 * 
 * @param value Entero a guardar
 * 
 * Serializa el entero en formato binario usando el operador << de DataBuffer.
 */
void Memento::Snapshot::save_int(int value)
{
    buffer_ << value;
}

/**
 * @brief Cargar un entero desde el snapshot
 * 
 * @return Entero cargado
 * 
 * Deserializa un entero previamente guardado usando el operador >> de DataBuffer.
 * 
 * @throw std::runtime_error si no hay un entero válido para cargar
 */
int Memento::Snapshot::load_int()
{
    int value;
    buffer_ >> value;
    return value;
}

/**
 * @brief Guardar un double en el snapshot
 * 
 * @param value Double a guardar
 * 
 * Serializa el double en formato binario usando el operador << de DataBuffer.
 */
void Memento::Snapshot::save_double(double value)
{
    buffer_ << value;
}

/**
 * @brief Cargar un double desde el snapshot
 * 
 * @return Double cargado
 * 
 * Deserializa un double previamente guardado usando el operador >> de DataBuffer.
 * 
 * @throw std::runtime_error si no hay un double válido para cargar
 */
double Memento::Snapshot::load_double()
{
    double value;
    buffer_ >> value;
    return value;
}

/**
 * @brief Guardar un booleano en el snapshot
 * 
 * @param value Booleano a guardar
 * 
 * Serializa el booleano en formato binario usando el operador << de DataBuffer.
 */
void Memento::Snapshot::save_bool(bool value)
{
    buffer_ << value;
}

/**
 * @brief Cargar un booleano desde el snapshot
 * 
 * @return Booleano cargado
 * 
 * Deserializa un booleano previamente guardado usando el operador >> de DataBuffer.
 * 
 * @throw std::runtime_error si no hay un booleano válido para cargar
 */
bool Memento::Snapshot::load_bool()
{
    bool value;
    buffer_ >> value;
    return value;
}

/**
 * @brief Verificar si el snapshot está vacío
 * 
 * @return true si no contiene datos, false en caso contrario
 * 
 * Delega en el método empty() de DataBuffer para determinar si hay datos.
 */
bool Memento::Snapshot::empty() const
{
    return buffer_.empty();
}

/**
 * @brief Limpiar todo el contenido del snapshot
 * 
 * Elimina todos los datos serializados y deja el snapshot listo para reutilizar.
 * Delega en el método clear() de DataBuffer.
 */
void Memento::Snapshot::clear()
{
    buffer_.clear();
}

/**
 * @brief Obtener referencia al buffer interno (no constante)
 * 
 * @return DataBuffer& Referencia al DataBuffer interno
 * 
 * @note Este método es privado y solo accesible por Memento
 */
DataBuffer& Memento::Snapshot::get_buffer()
{
    return buffer_;
}

/**
 * @brief Obtener referencia constante al buffer interno
 * 
 * @return const DataBuffer& Referencia constante al DataBuffer interno
 * 
 * @note Este método es privado y solo accesible por Memento
 */
const DataBuffer& Memento::Snapshot::get_buffer() const
{
    return buffer_;
}

// ============ IMPLEMENTACIÓN DE LA CLASE MEMENTO ============

/**
 * @brief Destructor virtual de Memento
 * 
 * Proporciona una implementación por defecto para el destructor virtual
 * que permite una destrucción segura de clases derivadas.
 */
Memento::~Memento()
{
}

/**
 * @brief Guardar el estado actual del objeto
 * 
 * @return Snapshot que contiene el estado serializado
 * 
 * Este método:
 * 1. Crea un Snapshot vacío
 * 2. Llama al método protegido _saveToSnapshot para que la clase derivada
 *    serialice su estado en el snapshot
 * 3. Retorna el snapshot con el estado capturado
 */
Memento::Snapshot Memento::save() const
{
    Snapshot snapshot;
    _saveToSnapshot(snapshot);
    return snapshot;
}

/**
 * @brief Cargar un estado previamente guardado
 * 
 * @param state Snapshot que contiene el estado a restaurar
 * 
 * Este método:
 * 1. Crea una copia no-constante del snapshot (para permitir lectura)
 * 2. Llama al método protegido _loadFromSnapshot para que la clase derivada
 *    deserialice y restaure su estado desde el snapshot
 * 
 * @note Se crea una copia para no modificar el snapshot original
 */
void Memento::load(const Snapshot& state)
{
    // Crear copia no-constante para permitir operaciones de lectura
    Snapshot non_const_state = state;
    _loadFromSnapshot(non_const_state);
}