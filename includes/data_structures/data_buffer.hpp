#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

#include <vector>
#include <stdexcept>
#include <string>

/**
 * @class DataBuffer
 * @brief Contenedor polimórfico para almacenar objetos en formato de bytes.
 * 
 * Esta clase proporciona un buffer dinámico que permite serializar y deserializar
 * objetos de diferentes tipos en formato binario. Utiliza un vector de caracteres
 * como almacenamiento subyacente y mantiene posiciones separadas para lectura y escritura.
 * 
 * Características principales:
 * - Serialización/deserialización de tipos básicos y objetos complejos
 * - Operadores << y >> para una API intuitiva
 * - Gestión automática de memoria
 * - Control de errores con excepciones
 * - Compatibilidad con C++11
 */
class DataBuffer {
private:
    std::vector<char> buffer;  ///< Almacenamiento interno de datos en formato de bytes
    size_t read_pos;           ///< Posición actual para operaciones de lectura
    size_t write_pos;          ///< Posición actual para operaciones de escritura

public:
    // ============ CONSTRUCTORES Y DESTRUCTOR ============
    
    /**
     * @brief Constructor por defecto.
     * Inicializa un buffer vacío con posiciones de lectura/escritura en 0.
     */
    DataBuffer();
    
    /**
     * @brief Constructor de copia.
     * @param other Buffer del cual copiar el contenido y estado.
     */
    DataBuffer(const DataBuffer& other);
    
    /**
     * @brief Operador de asignación.
     * @param other Buffer del cual copiar el contenido y estado.
     * @return Referencia a este objeto para encadenamiento.
     */
    DataBuffer& operator=(const DataBuffer& other);
    
    /**
     * @brief Destructor.
     * Libera automáticamente todos los recursos gracias al uso de std::vector.
     */
    ~DataBuffer();

    // ============ MÉTODOS BÁSICOS DE MANEJO DEL BUFFER ============
    
    /**
     * @brief Añade una cadena de texto al buffer.
     * @param str Cadena a añadir al final del buffer.
     * 
     * @note Este método es útil para usar el DataBuffer como un buffer de texto simple.
     */
    void append(const std::string& str);
    
    /**
     * @brief Convierte el contenido del buffer a una cadena de texto.
     * @return std::string con todo el contenido escrito en el buffer.
     * 
     * @note Solo incluye los datos hasta la posición de escritura actual.
     */
    std::string str() const;
    
    /**
     * @brief Limpia completamente el buffer.
     * 
     * Reinicia todas las posiciones y libera la memoria del vector.
     * El buffer queda listo para ser reutilizado.
     */
    void clear();
    
    /**
     * @brief Verifica si el buffer está vacío.
     * @return true si no hay datos escritos, false en caso contrario.
     */
    bool empty() const;
    
    /**
     * @brief Obtiene el tamaño actual de los datos escritos.
     * @return Número de bytes actualmente en el buffer.
     */
    size_t size() const;

    // ============ OPERADORES DE SERIALIZACIÓN GENÉRICOS ============
    
    /**
     * @brief Operador de inserción para serializar cualquier tipo de dato.
     * @tparam T Tipo del dato a serializar (debe ser serializable).
     * @param data Dato a serializar en el buffer.
     * @return Referencia a este buffer para permitir encadenamiento.
     * 
     * @note Para tipos básicos (int, float, etc.) se usa copia directa de memoria.
     * Para tipos complejos, debe existir una especialización adecuada.
     */
    template<typename T>
    DataBuffer& operator<<(const T& data);
    
    /**
     * @brief Operador de extracción para deserializar cualquier tipo de dato.
     * @tparam T Tipo del dato a deserializar.
     * @param data Variable donde se almacenará el dato deserializado.
     * @return Referencia a este buffer para permitir encadenamiento.
     * 
     * @throw std::runtime_error si no hay suficientes datos para leer.
     */
    template<typename T>
    DataBuffer& operator>>(T& data);
    
    // ============ ESPECIALIZACIONES PARA std::string ============
    
    /**
     * @brief Operador de inserción especializado para cadenas.
     * @param data Cadena a serializar en el buffer.
     * @return Referencia a este buffer para permitir encadenamiento.
     * 
     * @note Las cadenas se serializan con su longitud primero, seguida del contenido.
     */
    DataBuffer& operator<<(const std::string& data);
    
    /**
     * @brief Operador de extracción especializado para cadenas.
     * @param data Cadena donde se almacenará el resultado.
     * @return Referencia a este buffer para permitir encadenamiento.
     * 
     * @throw std::runtime_error si no hay suficientes datos para leer la cadena completa.
     */
    DataBuffer& operator>>(std::string& data);

private:
    // ============ MÉTODOS PRIVADOS DE BAJO NIVEL ============
    
    /**
     * @brief Garantiza que el buffer tenga capacidad suficiente.
     * @param needed Número mínimo de bytes necesarios.
     * 
     * Redimensiona el vector interno si es necesario para acomodar los bytes requeridos.
     */
    void ensure_capacity(size_t needed);
    
    /**
     * @brief Escribe un bloque de bytes en el buffer.
     * @param data Puntero al inicio de los datos a escribir.
     * @param size Número de bytes a escribir.
     * 
     * @note Este método actualiza automáticamente la posición de escritura.
     */
    void write_bytes(const char* data, size_t size);
    
    /**
     * @brief Lee un bloque de bytes del buffer.
     * @param data Puntero donde almacenar los bytes leídos.
     * @param size Número de bytes a leer.
     * 
     * @throw std::runtime_error si se intenta leer más allá del final del buffer.
     * @note Este método actualiza automáticamente la posición de lectura.
     */
    void read_bytes(char* data, size_t size);
};

// Incluir implementaciones de templates al final del header
#include "data_buffer.tpp"

#endif // DATA_BUFFER_HPP