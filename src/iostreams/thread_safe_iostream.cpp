#include "iostreams/thread_safe_iostream.hpp"
#include <iostream>
#include <map>
#include <thread>

// ============ DEFINICIÓN DE VARIABLES ESTÁTICAS ============

std::mutex ThreadSafeIOStream::_coutMutex;
std::mutex ThreadSafeIOStream::_cinMutex;

// ============ VARIABLES THREAD-LOCAL ============

// Almacenamos el buffer y prefijo por hilo usando thread_local
thread_local std::ostringstream local_thread_buffer;
thread_local std::string local_thread_prefix;

// ============ INSTANCIA GLOBAL ÚNICA ============

ThreadSafeIOStream threadSafeCout;

// ============ IMPLEMENTACIÓN DE MÉTODOS ============

ThreadSafeIOStream::ThreadSafeIOStream() {
    _stringPool.resize(100);
    initializeStateMachine();
    
    // Configurar observers
    _observer.subscribe(StreamEvent::LINE_PRINTED, [](const std::string& message) {
        (void)message;
    });
    
    _observer.subscribe(StreamEvent::PREFIX_CHANGED, [](const std::string& prefix) {
        (void)prefix;
    });
    
    _observer.subscribe(StreamEvent::STREAM_FLUSHED, [](const std::string& info) {
        (void)info;
    });
}

ThreadSafeIOStream::~ThreadSafeIOStream() {
    flush();
}

void ThreadSafeIOStream::initializeStateMachine() {
    _stateMachine.addState(StreamState::IDLE);
    _stateMachine.addState(StreamState::BUFFERING);
    _stateMachine.addState(StreamState::FLUSHING);
    
    _stateMachine.addTransition(StreamState::IDLE, StreamState::BUFFERING, []() {});
    _stateMachine.addTransition(StreamState::BUFFERING, StreamState::FLUSHING, []() {});
    _stateMachine.addTransition(StreamState::FLUSHING, StreamState::IDLE, []() {});
    _stateMachine.addTransition(StreamState::FLUSHING, StreamState::BUFFERING, []() {});
}

std::ostringstream& ThreadSafeIOStream::getLocalBuffer() {
    return local_thread_buffer;
}

std::string& ThreadSafeIOStream::getLocalPrefix() {
    return local_thread_prefix;
}

void ThreadSafeIOStream::setPrefix(const std::string& prefix) {
    getLocalPrefix() = prefix;
    _observer.notify(StreamEvent::PREFIX_CHANGED, prefix);
    _dataBuffer << "[PREFIX_CHANGE] " << prefix;
}

std::string ThreadSafeIOStream::getPrefix() const {
    return local_thread_prefix;
}

ThreadSafeIOStream& ThreadSafeIOStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
        // Obtener el contenido del buffer local del hilo
        std::string content = getLocalBuffer().str();
        getLocalBuffer().str("");
        getLocalBuffer().clear();
        
        if (!content.empty()) {
            // Crear la línea completa con prefijo del hilo
            std::string fullLine = getLocalPrefix() + content;
            
            // Imprimir de manera atómica con el mutex ESTÁTICO (compartido)
            std::lock_guard<std::mutex> lock(_coutMutex);
            std::cout << fullLine << std::endl;
            
            _observer.notify(StreamEvent::LINE_PRINTED, fullLine);
            _dataBuffer << "[OUTPUT] " << fullLine;
        } else {
            // Si no hay contenido, solo imprimir nueva línea
            std::lock_guard<std::mutex> lock(_coutMutex);
            std::cout << std::endl;
        }
        
        // Cambiar estados
        if (_stateMachine.canTransitionTo(StreamState::IDLE)) {
            _stateMachine.transitionTo(StreamState::IDLE);
        }
    } else if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::flush)) {
        flush();
    } else {
        getLocalBuffer() << manip;
    }
    return *this;
}

void ThreadSafeIOStream::flush() {
    std::string content = getLocalBuffer().str();
    if (!content.empty()) {
        std::string fullLine = getLocalPrefix() + content;
        
        // Usar el mutex ESTÁTICO (compartido)
        std::lock_guard<std::mutex> lock(_coutMutex);
        std::cout << fullLine << std::flush;
        
        _observer.notify(StreamEvent::LINE_PRINTED, fullLine);
        _dataBuffer << "[OUTPUT] " << fullLine;
        
        getLocalBuffer().str("");
        getLocalBuffer().clear();
    }
    
    if (_stateMachine.canTransitionTo(StreamState::IDLE)) {
        _stateMachine.transitionTo(StreamState::IDLE);
    }
    
    _observer.notify(StreamEvent::STREAM_FLUSHED, "[MANUAL_FLUSH]");
}

void ThreadSafeIOStream::subscribeToEvent(StreamEvent event, const std::function<void(const std::string&)>& callback) {
    _observer.subscribe(event, callback);
}

Memento::Snapshot ThreadSafeIOStream::saveState() {
    Memento::Snapshot snapshot;
    snapshot << getLocalPrefix();
    snapshot << static_cast<int>(_stateMachine.getCurrentState());
    std::string bufferContent = _dataBuffer.str();
    snapshot << bufferContent;
    return snapshot;
}

void ThreadSafeIOStream::restoreState(const Memento::Snapshot& snapshot) {
    Memento::Snapshot nonConstSnapshot = snapshot;
    std::string savedPrefix;
    nonConstSnapshot >> savedPrefix;
    getLocalPrefix() = savedPrefix;
    
    int savedState;
    nonConstSnapshot >> savedState;
    StreamState state = static_cast<StreamState>(savedState);
    
    if (_stateMachine.canTransitionTo(state)) {
        _stateMachine.transitionTo(state);
    }
    
    std::string bufferContent;
    nonConstSnapshot >> bufferContent;
    _dataBuffer.clear();
    _dataBuffer.append(bufferContent);
    
    _observer.notify(StreamEvent::PREFIX_CHANGED, "[RESTORED] " + savedPrefix);
}

StreamState ThreadSafeIOStream::getCurrentState() const {
    return _stateMachine.getCurrentState();
}