/**
 * @file thread_safe_queue.cpp
 * @brief Instanciaciones explícitas de ThreadSafeQueue para tipos comunes
 * 
 * Este archivo proporciona instanciaciones explícitas de la plantilla
 * ThreadSafeQueue para los tipos de datos más comunes, asegurando que
 * el linker encuentre las implementaciones compiladas.
 * 
 * TIPOS INSTANCIADOS:
 * - int: Para colas de enteros
 * - std::string: Para colas de cadenas de texto
 * 
 * BENEFICIOS:
 * - Reduce tiempos de compilación
 * - Evita problemas de linker con templates
 * - Proporciona binarios más optimizados
 */

#include "threading/thread_safe_queue.hpp"

// Instanciación explícita para colas de enteros
template class ThreadSafeQueue<int>;

// Instanciación explícita para colas de cadenas de texto
template class ThreadSafeQueue<std::string>;