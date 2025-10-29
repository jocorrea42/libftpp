#ifndef STATE_MACHINE_TPP
#define STATE_MACHINE_TPP

#include "state_machine.hpp"

// ============ IMPLEMENTACIONES DE MÉTODOS TEMPLATE ============

/**
 * @brief Constructor por defecto
 * 
 * Inicializa la máquina de estados sin estado inicial.
 * El flag has_initial_state_ se establece en false hasta que
 * se añada el primer estado.
 */
template<typename TState>
StateMachine<TState>::StateMachine() : has_initial_state_(false) {}

/**
 * @brief Verifica si un estado es válido en la máquina
 * 
 * @param state Estado a verificar
 * @return true si el estado existe en la lista de estados válidos
 * 
 * Este método recorre la lista de estados válidos para determinar
 * si el estado proporcionado ha sido registrado previamente.
 */
template<typename TState>
bool StateMachine<TState>::isValidState(const TState& state) const {
    for (const auto& valid_state : valid_states_) {
        if (valid_state == state) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Valida que un estado exista en la máquina
 * 
 * @param state Estado a validar
 * @throw std::invalid_argument si el estado no ha sido añadido previamente
 * 
 * Utiliza isValidState() para verificar la existencia y lanza una excepción
 * descriptiva si el estado no es válido.
 */
template<typename TState>
void StateMachine<TState>::validateState(const TState& state) const {
    if (!isValidState(state)) {
        throw std::invalid_argument("StateMachine: state not found");
    }
}

/**
 * @brief Añade un estado posible a la máquina
 * 
 * @param state Estado a añadir
 * @throw std::invalid_argument si el estado ya existe
 * 
 * Este método:
 * 1. Verifica que el estado no exista previamente
 * 2. Añade el estado a la lista de estados válidos
 * 3. Si es el primer estado, lo establece como estado inicial
 */
template<typename TState>
void StateMachine<TState>::addState(const TState& state) {
    if (isValidState(state)) {
        throw std::invalid_argument("StateMachine: state already exists");
    }
    
    valid_states_.push_back(state);
    
    // Establecer el primer estado añadido como estado inicial
    if (!has_initial_state_) {
        current_state_ = state;
        has_initial_state_ = true;
    }
}

/**
 * @brief Especifica qué ejecutar durante una transición específica
 * 
 * @param startState Estado de origen de la transición
 * @param finalState Estado de destino de la transición
 * @param lambda Función a ejecutar durante la transición
 * @throw std::invalid_argument si alguno de los estados no existe
 * 
 * Las transiciones se almacenan en un mapa donde la clave es un par
 * (estado_origen, estado_destino) y el valor es la función callback.
 */
template<typename TState>
void StateMachine<TState>::addTransition(const TState& startState, const TState& finalState, 
                      const std::function<void()>& lambda) {
    validateState(startState);
    validateState(finalState);
    
    std::pair<TState, TState> transition = std::make_pair(startState, finalState);
    transitions_[transition] = lambda;
}

/**
 * @brief Especifica qué ejecutar cuando la máquina está en un estado específico
 * 
 * @param state Estado para el cual registrar la acción
 * @param lambda Función a ejecutar durante update()
 * @throw std::invalid_argument si el estado no existe
 * 
 * Las acciones se almacenan en un mapa donde la clave es el estado
 * y el valor es la función a ejecutar cuando la máquina está en ese estado.
 */
template<typename TState>
void StateMachine<TState>::addAction(const TState& state, const std::function<void()>& lambda) {
    validateState(state);
    state_actions_[state] = lambda;
}

/**
 * @brief Ejecuta la transición a un estado específico
 * 
 * @param state Estado destino de la transición
 * @throw std::invalid_argument si no hay estado inicial, el estado no existe
 *        o no hay transición definida desde el estado actual
 * 
 * Este método realiza las siguientes verificaciones:
 * 1. Existencia de estado inicial
 * 2. Validez del estado destino
 * 3. Existencia de transición desde el estado actual al destino
 * 
 * Si todas las verificaciones pasan, ejecuta el callback de transición
 * y actualiza el estado actual.
 */
template<typename TState>
void StateMachine<TState>::transitionTo(const TState& state) {
    if (!has_initial_state_) {
        throw std::invalid_argument("StateMachine: no initial state set");
    }
    
    validateState(state);
    
    std::pair<TState, TState> transition = std::make_pair(current_state_, state);
    auto transition_it = transitions_.find(transition);
    
    if (transition_it == transitions_.end()) {
        throw std::invalid_argument("StateMachine: no transition defined");
    }
    
    // Ejecutar el callback de transición si existe
    if (transition_it->second) {
        transition_it->second();
    }
    
    // Actualizar el estado actual
    current_state_ = state;
}

/**
 * @brief Ejecuta la acción registrada para el estado actual
 * 
 * @throw std::invalid_argument si no hay estado inicial establecido
 * 
 * Este método:
 * 1. Verifica que haya un estado inicial
 * 2. Valida el estado actual
 * 3. Busca y ejecuta la acción asociada al estado actual
 * 
 * Si no hay acción registrada para el estado actual, no se ejecuta nada
 * pero no se considera un error.
 */
template<typename TState>
void StateMachine<TState>::update() {
    if (!has_initial_state_) {
        throw std::invalid_argument("StateMachine: no initial state set");
    }
    
    validateState(current_state_);
    
    auto action_it = state_actions_.find(current_state_);
    if (action_it != state_actions_.end() && action_it->second) {
        action_it->second();
    }
}

/**
 * @brief Obtiene el estado actual de la máquina
 * 
 * @return Estado actual
 * @throw std::invalid_argument si no hay estado inicial establecido
 */
template<typename TState>
TState StateMachine<TState>::getCurrentState() const { 
    if (!has_initial_state_) {
        throw std::invalid_argument("StateMachine: no initial state set");
    }
    return current_state_; 
}

/**
 * @brief Verifica si es posible transicionar a un estado desde el estado actual
 * 
 * @param state Estado destino a verificar
 * @return true si existe una transición válida, false en caso contrario
 * 
 * Este método verifica:
 * 1. Existencia de estado inicial
 * 2. Validez del estado destino  
 * 3. Existencia de transición en el mapa de transiciones
 */
template<typename TState>
bool StateMachine<TState>::canTransitionTo(const TState& state) const {
    if (!has_initial_state_ || !isValidState(state)) {
        return false;
    }
    
    std::pair<TState, TState> transition = std::make_pair(current_state_, state);
    return transitions_.find(transition) != transitions_.end();
}

/**
 * @brief Verifica si un estado existe en la máquina
 * 
 * @param state Estado a verificar
 * @return true si el estado existe, false en caso contrario
 */
template<typename TState>
bool StateMachine<TState>::hasState(const TState& state) const {
    return isValidState(state);
}

/**
 * @brief Obtiene el número de estados en la máquina
 * 
 * @return Número de estados registrados
 */
template<typename TState>
size_t StateMachine<TState>::getStateCount() const {
    return valid_states_.size();
}

/**
 * @brief Obtiene el número de transiciones definidas
 * 
 * @return Número de transiciones registradas
 */
template<typename TState>
size_t StateMachine<TState>::getTransitionCount() const {
    return transitions_.size();
}

#endif // STATE_MACHINE_TPP