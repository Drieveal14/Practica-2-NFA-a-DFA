#include "automata.h"

#include <iostream>

int main() {
    automata::AFN afn;
    afn.inicial = 0;
    afn.finales = {2};
    afn.agregarEpsilon(0, 1);
    afn.agregarTransicion(1, 'a', 2);

    // AFD equivalente construido a mano; aqui no se ejecuta la conversion.
    automata::AFD afd;
    afd.alfabeto = afn.alfabeto;
    afd.inicial = afd.agregarEstado({0, 1});
    int fin = afd.agregarEstado({2});
    afd.finales = {fin};
    afd.agregarTransicion(afd.inicial, 'a', fin);

    afn.imprimirTabla(std::cout);
    std::cout << '\n';
    afd.imprimirTabla(std::cout);
}
