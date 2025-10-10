#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP


#include <map>
#include <functional>
#include <stdexcept>


namespace ft {


template <typename TState>
class StateMachine {
public:
void addState(const TState& state) {
_states[state] = {};
}


void addTransition(const TState& start, const TState& end, const std::function<void()>& lambda) {
_transitions[{start, end}] = lambda;
}


void addAction(const TState& state, const std::function<void()>& lambda) {
_actions[state] = lambda;
}


void transitionTo(const TState& state) {
if (!_transitions.count({_currentState, state}))
throw std::runtime_error("Transition not defined!");
_transitions[{_currentState, state}]();
_currentState = state;
}


void update() {
if (!_actions.count(_currentState))
throw std::runtime_error("Action not defined for current state!");
_actions[_currentState]();
}


private:
	TState _currentState = TState();
std::map<TState, std::function<void()>> _actions;
std::map<std::pair<TState, TState>, std::function<void()>> _transitions;
std::map<TState, bool> _states;
};


} // namespace ft


#endif // STATE_MACHINE_HPP