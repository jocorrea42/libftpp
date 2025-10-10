#include "libftpp.hpp"
#include <iostream>

int main() {
    ft::threadSafeCout.setPrefix("[StateMachineTest] ");

    enum class State { Idle, Running, Stopped };

    ft::StateMachine<State> sm;
    sm.addState(State::Idle);
    sm.addState(State::Running);
    sm.addState(State::Stopped);

    bool enteredRunning = false;
    bool enteredStopped = false;

    sm.addAction(State::Idle, [](){ /* nothing */ });
    sm.addAction(State::Running, [&](){ enteredRunning = true; });
    sm.addAction(State::Stopped, [&](){ enteredStopped = true; });

    sm.addTransition(State::Idle, State::Running, [](){});
    sm.addTransition(State::Running, State::Stopped, [](){});

    // initialize current state to Idle
    sm.addTransition(State::Idle, State::Idle, [](){});
    sm.transitionTo(State::Idle);

    // Now transition to Running
    sm.transitionTo(State::Running);
    sm.update();
    if (!enteredRunning) {
        ft::threadSafeCout << "FAIL: action for Running not executed" << ft::threadSafeCout.endl();
        return 1;
    }

    sm.transitionTo(State::Stopped);
    sm.update();
    if (!enteredStopped) {
        ft::threadSafeCout << "FAIL: action for Stopped not executed" << ft::threadSafeCout.endl();
        return 1;
    }

    ft::threadSafeCout << "StateMachine tests passed." << ft::threadSafeCout.endl();
    return 0;
}
