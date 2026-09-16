#include "automata.h"
#include "operations.h"

#include <iostream>
#include <stdexcept>

namespace {

// Si la condicion es falsa, interrumpe la prueba con un mensaje descriptivo
void comprobar(bool condicion, const char* mensaje) {
    if (!condicion) throw std::runtime_error(mensaje);
}

// Construye el AFN clasico de (a|b)*abb (Aho, Sethi, Ullman) para probar
// move con varios estados y varios simbolos a la vez.
automata::AFN construirAfnAbb() {
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

// move(T,a) debe ser la union de destinos con ese simbolo para cada estado
// de T, y el conjunto vacio si ningun estado tiene esa transicion.
void probarMove() {
    automata::AFN afn = construirAfnAbb();

    // Desde {7} con 'a' solo hay un destino directo: {8}
    comprobar(automata::move(afn, {7}, 'a') == automata::ConjuntoEstados({8}),
              "move de un solo estado incorrecto");

    // Desde {3,5} con 'a' solo 3 tiene transicion (a 6); 5 no aporta nada
    comprobar(automata::move(afn, {3, 5}, 'a') == automata::ConjuntoEstados({6}),
              "move no une correctamente varios estados");

    // Desde {3,5} con 'b' solo 5 tiene transicion (a 6)
    comprobar(automata::move(afn, {3, 5}, 'b') == automata::ConjuntoEstados({6}),
              "move no distingue simbolos");

    // Ningun estado en T tiene transicion con 'a': resultado vacio
    comprobar(automata::move(afn, {9, 10}, 'a').empty(),
              "move deberia ser vacio sin transiciones");

    // move sobre el conjunto vacio siempre es vacio
    comprobar(automata::move(afn, {}, 'a').empty(),
              "move del conjunto vacio deberia ser vacio");
}

// epsilon-closure debe incluir los estados originales y alcanzar todo lo
// conectado por epsilon, sin quedarse ciclando ni repetir trabajo.
void probarEpsilonClosure() {
    automata::AFN afn = construirAfnAbb();

    // Clausura del estado inicial: el ejemplo clasico da {0,1,2,4,3,5,7}... es
    // decir, todo lo alcanzable por epsilon desde 0 antes de consumir nada.
    automata::ConjuntoEstados esperado = {0, 1, 2, 3, 4, 5, 6, 7};
    // Nota: 6 no es alcanzable por epsilon puro desde 0 (hace falta 'a' o 'b'
    // primero), asi que lo quitamos del esperado.
    esperado.erase(6);
    comprobar(automata::epsilonClosure(afn, {0}) == esperado,
              "epsilon-closure del inicial incorrecta");

    // Version de un solo estado (sobrecarga con int) debe dar lo mismo
    comprobar(automata::epsilonClosure(afn, 0) == esperado,
              "sobrecarga epsilonClosure(afn, estado) inconsistente");

    // La clausura de un estado sin transiciones epsilon salientes es el
    // mismo estado
    comprobar(automata::epsilonClosure(afn, {10}) == automata::ConjuntoEstados({10}),
              "epsilon-closure de un estado final incorrecta");

    // El conjunto vacio no tiene estados que expandir
    // (se usa una variable explicita porque {} es ambiguo entre las dos
    // sobrecargas de epsilonClosure: ConjuntoEstados vs int)
    const automata::ConjuntoEstados vacio;
    comprobar(automata::epsilonClosure(afn, vacio).empty(),
              "epsilon-closure del conjunto vacio deberia ser vacio");

    // Un ciclo epsilon (0 <-> 2 <-> 0) no debe provocar un bucle infinito y
    // debe reconocer que ambos estados son mutuamente alcanzables
    automata::AFN conCiclo;
    conCiclo.agregarEpsilon(0, 2);
    conCiclo.agregarEpsilon(2, 0);
    comprobar(automata::epsilonClosure(conCiclo, {0}) == automata::ConjuntoEstados({0, 2}),
              "epsilon-closure no maneja ciclos correctamente");
}

}  // namespace

int main() {
    try {
        probarMove();
        probarEpsilonClosure();
        std::cout << "OK: move y epsilon-closure\n";
    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }
}
