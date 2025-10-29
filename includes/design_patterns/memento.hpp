#ifndef MEMENTO_HPP
#define MEMENTO_HPP

#include "data_structures/data_buffer.hpp"
#include <stdexcept>
#include <vector>
#include <string>

/**
 * @class Memento
 * @brief Implementación del patrón Memento para guardar y restaurar el estado de objetos.
 * 
 * Este patrón permite capturar y externalizar el estado interno de un objeto sin violar
 * el encapsulamiento, de modo que el objeto pueda ser restaurado a este estado más tarde.
 * 
 * Características principales:
 * - Permite guardar y cargar estados de objetos de forma segura
 * - Utiliza DataBuffer para serialización polimórfica
 * - Proporciona una interfaz clara para clases "saveable"
 * - Mantiene el encapsulamiento mediante métodos protegidos
 * 
 * Las clases que deseen ser "saveable" deben:
 * 1. Heredar de Memento
 * 2. Implementar los métodos protegidos _saveToSnapshot y _loadFromSnapshot
 * 3. Declarar a Memento como friend para permitir acceso a métodos privados
 */
class Memento
{
public:
    // ============ CLASE INTERNA SNAPSHOT ============
    
    /**
     * @class Snapshot
     * @brief Almacena el estado serializado de un objeto en un momento específico.
     * 
     * Esta clase encapsula el estado serializado utilizando DataBuffer y proporciona
     * una interfaz amigable para guardar y cargar diferentes tipos de datos.
     * 
     * El Snapshot actúa como el "recuerdo" en el patrón Memento, almacenando el estado
     * de forma que pueda ser restaurado posteriormente sin conocer los detalles internos.
     */
    class Snapshot
    {
    private:
        DataBuffer buffer_;  ///< Buffer interno para almacenamiento serializado

    public:
        // ============ CONSTRUCTORES Y DESTRUCTOR ============
        
        /**
         * @brief Constructor por defecto
         */
        Snapshot();
        
        /**
         * @brief Constructor de copia
         * @param other Otro Snapshot a copiar
         */
        Snapshot(const Snapshot& other);
        
        /**
         * @brief Operador de asignación
         * @param other Otro Snapshot a copiar
         * @return Referencia a este Snapshot
         */
        Snapshot& operator=(const Snapshot& other);
        
        /**
         * @brief Destructor
         */
        ~Snapshot();

        // ============ OPERADORES DE SERIALIZACIÓN GENÉRICOS ============
        
        /**
         * @brief Operador de inserción para serializar datos
         * @tparam T Tipo del dato a serializar
         * @param data Dato a serializar
         * @return Referencia a este Snapshot para encadenamiento
         */
        template<typename T>
        Snapshot& operator<<(const T& data);
        
        /**
         * @brief Operador de extracción para deserializar datos
         * @tparam T Tipo del dato a deserializar
         * @param data Variable donde almacenar el dato
         * @return Referencia a este Snapshot para encadenamiento
         */
        template<typename T>
        Snapshot& operator>>(T& data);
        
        // ============ MÉTODOS ESPECÍFICOS POR TIPO ============
        
        /**
         * @brief Guardar una cadena en el snapshot
         * @param str Cadena a guardar
         */
        void save_string(const std::string& str);
        
        /**
         * @brief Cargar una cadena desde el snapshot
         * @param str Cadena donde almacenar el resultado
         */
        void load_string(std::string& str);
        
        /**
         * @brief Guardar un entero en el snapshot
         * @param value Entero a guardar
         */
        void save_int(int value);
        
        /**
         * @brief Cargar un entero desde el snapshot
         * @return Entero cargado
         */
        int load_int();
        
        /**
         * @brief Guardar un double en el snapshot
         * @param value Double a guardar
         */
        void save_double(double value);
        
        /**
         * @brief Cargar un double desde el snapshot
         * @return Double cargado
         */
        double load_double();
        
        /**
         * @brief Guardar un booleano en el snapshot
         * @param value Booleano a guardar
         */
        void save_bool(bool value);
        
        /**
         * @brief Cargar un booleano desde el snapshot
         * @return Booleano cargado
         */
        bool load_bool();

        // ============ MÉTODOS DE GESTIÓN DEL SNAPSHOT ============
        
        /**
         * @brief Verificar si el snapshot está vacío
         * @return true si no contiene datos, false en caso contrario
         */
        bool empty() const;
        
        /**
         * @brief Limpiar todo el contenido del snapshot
         */
        void clear();

    private:
        // ============ MÉTODOS PRIVADOS ============
        
        /**
         * @brief Obtener referencia al buffer interno (no constante)
         * @return Referencia al DataBuffer interno
         * 
         * @note Solo Memento puede acceder a este método
         */
        DataBuffer& get_buffer();
        
        /**
         * @brief Obtener referencia constante al buffer interno
         * @return Referencia constante al DataBuffer interno
         * 
         * @note Solo Memento puede acceder a este método
         */
        const DataBuffer& get_buffer() const;
        
        // Amistad para permitir a Memento acceder a los métodos privados
        friend class Memento;
    };

    // ============ DESTRUCTOR VIRTUAL ============
    
    /**
     * @brief Destructor virtual
     */
    virtual ~Memento();

    // ============ MÉTODOS PÚBLICOS PARA GUARDAR/CARGAR ESTADO ============
    
    /**
     * @brief Guardar el estado actual del objeto
     * @return Snapshot que contiene el estado serializado
     * 
     * Este método es la interfaz pública para capturar el estado actual.
     * Delega en el método protegido _saveToSnapshot implementado por la clase derivada.
     */
    Snapshot save() const;
    
    /**
     * @brief Cargar un estado previamente guardado
     * @param state Snapshot que contiene el estado a restaurar
     * 
     * Este método es la interfaz pública para restaurar un estado.
     * Delega en el método protegido _loadFromSnapshot implementado por la clase derivada.
     */
    void load(const Snapshot& state);

protected:
    // ============ MÉTODOS PROTEGIDOS A IMPLEMENTAR POR CLASES DERIVADAS ============
    
    /**
     * @brief Guardar el estado actual en un snapshot
     * @param snapshot Snapshot donde serializar el estado
     * 
     * Las clases derivadas deben implementar este método para especificar
     * qué datos deben ser guardados y en qué orden.
     */
    virtual void _saveToSnapshot(Snapshot& snapshot) const = 0;
    
    /**
     * @brief Cargar el estado desde un snapshot
     * @param snapshot Snapshot desde donde deserializar el estado
     * 
     * Las clases derivadas deben implementar este método para especificar
     * cómo restaurar los datos guardados previamente.
     * 
     * @note El orden de carga debe coincidir con el orden de guardado.
     */
    virtual void _loadFromSnapshot(Snapshot& snapshot) = 0;

private:
    // ============ CLASE AUXILIAR PARA ACCESO A MÉTODOS PROTEGIDOS ============
    
    /**
     * @class MementoAccess
     * @brief Clase auxiliar template para permitir acceso a métodos protegidos
     * 
     * Esta clase proporciona métodos estáticos para acceder a los métodos
     * protegidos _saveToSnapshot y _loadFromSnapshot de clases derivadas.
     * 
     * @tparam T Tipo de la clase derivada de Memento
     */
    template<typename T>
    friend class MementoAccess;
};

// ============ CLASE AUXILIAR MEMENTOACCESS ============

/**
 * @class MementoAccess
 * @brief Facilita el acceso a métodos protegidos de clases derivadas de Memento
 * 
 * Esta clase template actúa como un "puente" amigo que permite a Memento
 * acceder a los métodos protegidos de cualquier clase derivada, manteniendo
 * el encapsulamiento y la seguridad de tipos.
 * 
 * @tparam T Tipo específico de clase derivada de Memento
 */
template<typename T>
class MementoAccess
{
public:
    /**
     * @brief Guardar el estado de un objeto en un snapshot
     * @param object Objeto del cual guardar el estado
     * @param snapshot Snapshot donde guardar el estado
     */
    static void saveToSnapshot(const T& object, Memento::Snapshot& snapshot)
    {
        object._saveToSnapshot(snapshot);
    }

    /**
     * @brief Cargar el estado de un objeto desde un snapshot
     * @param object Objeto al cual cargar el estado
     * @param snapshot Snapshot desde donde cargar el estado
     */
    static void loadFromSnapshot(T& object, Memento::Snapshot& snapshot)
    {
        object._loadFromSnapshot(snapshot);
    }
};

// Incluir implementaciones de templates
#include "memento.tpp"

#endif // MEMENTO_HPP