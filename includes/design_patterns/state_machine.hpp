#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <map>
#include <functional>
#include <vector>
#include <stdexcept>
#include <utility>

/**
 * @class StateMachine
 * @brief Máquina de estados finita genérica para gestionar transiciones entre estados.
 * 
 * Esta plantilla implementa el patrón State Machine que permite definir un conjunto
 * de estados y las transiciones válidas entre ellos. Cada transición puede tener
 * un callback asociado, y cada estado puede tener una acción que se ejecuta
 * periódicamente.
 * 
 * Características principales:
 * - Gestión type-safe de estados mediante templates
 * - Transiciones con callbacks para lógica específica
 * - Acciones por estado para comportamiento continuo
 * - Validación estricta de estados y transiciones
 * - Manejo de excepciones para operaciones inválidas
 * - Estado inicial automático (primer estado añadido)
 * 
 * @tparam TState Tipo de los estados. Debe ser comparable (usualmente enum).
 * 
 * @example
 * enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER };
 * StateMachine<GameState> game;
 * 
 * game.addState(GameState::MENU);
 * game.addTransition(GameState::MENU, GameState::PLAYING, []() {
 *     std::cout << "Starting game..." << std::endl;
 * });
 * game.addAction(GameState::PLAYING, []() {
 *     std::cout << "Game is running" << std::endl;
 * });
 */
template<typename TState>
class StateMachine {
private:
    TState current_state_;                      ///< Estado actual de la máquina
    bool has_initial_state_;                   ///< Flag que indica si hay estado inicial establecido
    std::vector<TState> valid_states_;         ///< Lista de todos los estados válidos
    std::map<TState, std::function<void()>> state_actions_;           ///< Acciones por estado (ejecutadas en update())
    std::map<std::pair<TState, TState>, std::function<void()>> transitions_;  ///< Transiciones entre estados

    /**
     * @brief Verifica si un estado es válido (ha sido añadido previamente)
     * @param state Estado a verificar
     * @return true si el estado existe en la máquina, false en caso contrario
     */
    bool isValidState(const TState& state) const;

    /**
     * @brief Valida que un estado exista en la máquina
     * @param state Estado a validar
     * @throw std::invalid_argument si el estado no ha sido añadido previamente
     */
    void validateState(const TState& state) const;

public:
    // ============ CONSTRUCTOR ============
    
    /**
     * @brief Constructor por defecto
     * 
     * Inicializa una máquina de estados vacía sin estado inicial.
     * El estado inicial se establecerá automáticamente con el primer
     * estado añadido mediante addState().
     */
    StateMachine();

    // ============ MÉTODOS PÚBLICOS PRINCIPALES ============
    
    /**
     * @brief Añade un estado posible a la máquina
     * @param state Estado a añadir a la máquina
     * @throw std::invalid_argument si el estado ya existe
     * 
     * @note El primer estado añadido se establece automáticamente
     *       como estado inicial de la máquina.
     */
    void addState(const TState& state);
    
    /**
     * @brief Especifica qué ejecutar durante una transición específica
     * @param startState Estado de origen de la transición
     * @param finalState Estado de destino de la transición  
     * @param lambda Función a ejecutar cuando ocurre esta transición
     * @throw std::invalid_argument si alguno de los estados no existe
     * 
     * @note La transición solo se ejecutará cuando se llame a transitionTo()
     *       con el estado destino desde el estado origen correcto.
     */
    void addTransition(const TState& startState, const TState& finalState, 
                      const std::function<void()>& lambda);
    
    /**
     * @brief Especifica qué ejecutar cuando la máquina está en un estado específico
     * @param state Estado para el cual registrar la acción
     * @param lambda Función a ejecutar durante update() en este estado
     * @throw std::invalid_argument si el estado no existe
     * 
     * @note La acción se ejecuta cada vez que se llama a update() y la máquina
     *       se encuentra en el estado especificado.
     */
    void addAction(const TState& state, const std::function<void()>& lambda);
    
    /**
     * @brief Ejecuta la transición a un estado específico
     * @param state Estado destino de la transición
     * @throw std::invalid_argument si no hay estado inicial, el estado no existe
     *        o no hay transición definida desde el estado actual
     * 
     * Este método:
     * 1. Verifica que exista una transición válida desde el estado actual
     * 2. Ejecuta el callback asociado a la transición (si existe)
     * 3. Actualiza el estado actual al estado destino
     */
    void transitionTo(const TState& state);
    
    /**
     * @brief Ejecuta la acción registrada para el estado actual
     * @throw std::invalid_argument si no hay estado inicial establecido
     * 
     * @note Si no hay acción registrada para el estado actual, no se ejecuta nada
     *       pero no se lanza excepción (comportamiento por diseño).
     */
    void update();

    // ============ MÉTODOS DE CONSULTA ============
    
    /**
     * @brief Obtiene el estado actual de la máquina
     * @return Estado actual
     * @throw std::invalid_argument si no hay estado inicial establecido
     */
    TState getCurrentState() const;
    
    /**
     * @brief Verifica si es posible transicionar a un estado desde el estado actual
     * @param state Estado destino a verificar
     * @return true si existe una transición válida, false en caso contrario
     */
    bool canTransitionTo(const TState& state) const;
    
    /**
     * @brief Verifica si un estado existe en la máquina
     * @param state Estado a verificar
     * @return true si el estado existe, false en caso contrario
     */
    bool hasState(const TState& state) const;
    
    /**
     * @brief Obtiene el número de estados en la máquina
     * @return Número de estados registrados
     */
    size_t getStateCount() const;
    
    /**
     * @brief Obtiene el número de transiciones definidas
     * @return Número de transiciones registradas
     */
    size_t getTransitionCount() const;
};

// Incluir implementaciones de templates
#include "state_machine.tpp"

#endif // STATE_MACHINE_HPP