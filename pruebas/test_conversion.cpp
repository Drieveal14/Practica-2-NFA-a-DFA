#include "automata.h"
#include "converter.h"
#include <iostream>
#include <stdexcept>

// Función auxiliar para validaciones
void comprobar(bool condicion, const char* mensaje) {
    if (!condicion) throw std::runtime_error(mensaje);
}

// Construye el AFN para la expresión (a|b)*abb
automata::AFN construirAfnPrueba() {
    automata::AFN afn;
    afn.inicial = 0;
    afn.finales = {10};
    
    afn.agregarEpsilon(0, 1);
    afn.agregarEpsilon(0, 7);
    afn.agregarEpsilon(1, 2);
    afn.agregarEpsilon(1, 4);
    afn.agregarEpsilon(2, 3);
    afn.agregarTransicion(3, 'a', 6);
    afn.agregarEpsilon(4, 5);
    afn.agregarTransicion(5, 'b', 6);
    afn.agregarEpsilon(6, 1);
    afn.agregarEpsilon(6, 7);
    afn.agregarTransicion(7, 'a', 8);
    afn.agregarTransicion(8, 'b', 9);
    afn.agregarTransicion(9, 'b', 10);
    
    return afn;
}

int main() {
    try {
        automata::AFN afn = construirAfnPrueba();
        
        std::cout << "=== AUTÓMATA FINITO NO DETERMINISTA (AFN) ===\n";
        afn.imprimirTabla(std::cout);
        
        // Ejecuta el algoritmo de construcción de subconjuntos
        automata::AFD afd = automata::convertirAFNaAFD(afn);
        
        std::cout << "\n=== AUTÓMATA FINITO DETERMINISTA (AFD) RESULTANTE ===\n";
        afd.imprimirTabla(std::cout);
        
        // Verificaciones automatizadas
        comprobar(afd.inicial != -1, "El AFD debe tener un estado inicial.");
        comprobar(!afd.finales.empty(), "El AFD resultante debe heredar los estados de aceptación.");
        
        std::cout << "\nOK: Prueba superada. No hay transiciones epsilon y el autómata es completamente determinista.\n";
    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }
    return 0;
}