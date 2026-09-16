#include "automata.h"

#include <iostream>  
#include <sstream>   
#include <stdexcept>
#include <string>

namespace {

// Si la condicion es falsa, interrumpe la prueba con un mensaje descriptivo
void comprobar(bool condicion, const char* mensaje) {
    if (!condicion) throw std::runtime_error(mensaje);
}

// Comprueba que una operacion invalida lance el tipo de excepcion esperado
template <typename Excepcion, typename Accion>
void esperarExcepcion(Accion accion) {
    try {
        accion();
    } catch (const Excepcion&) {
        // Recibir la excpcion esperada significa que esta comprobacion paso
        return;
    }
    // Si no se lanzo ninguna excepcion, la operacion invalida no fue rechazada
    throw std::runtime_error("No se rechazo una operacion invalida");
}

// Verifica el almacenamiento y las consultas del AFN, no su conversion a AFD
void probarAFN() {
    automata::AFN afn;
    comprobar(afn.inicial == -1, "Inicial por defecto incorrecto");
    // El estado 0 sera inicial y final; inicialmente no hay transiciones
    afn.inicial = 0;
    afn.finales = {0};
    afn.alfabeto = {'a', 'b'};
    // Un mismo simbolo puede llevar a varios destinos en un AFN
    afn.agregarTransicion(0, 'a', 1);
    afn.agregarTransicion(0, 'a', 2);
    // Repetir esta transicion no debe duplicar el destino 1 en el conjunto
    afn.agregarTransicion(0, 'a', 1);
    // Se almacena un ciclo epsilon: 0 -> 2 -> 0, sin consumir simbolos
    afn.agregarEpsilon(0, 2);
    afn.agregarEpsilon(2, 0);
    // Las transiciones deben registrar automaticamente sus estados
    comprobar(afn.estados == automata::ConjuntoEstados({0, 1, 2}), "Registro de estados incorrecto");
    comprobar(afn.destinos(0, 'a') == automata::ConjuntoEstados({1, 2}), "Destinos AFN incorrectos");
    comprobar(afn.destinosEpsilon(0) == automata::ConjuntoEstados({2}), "Epsilon incorrecto");
    comprobar(afn.destinosEpsilon(2) == automata::ConjuntoEstados({0}), "Ciclo no representado");
    // Guardamos una copia para verificar que las consultas no cambian esta tabla
    const auto transicionesAntes = afn.transiciones;
    // Una transicion inexistente devuelve un conjunto vacio, sin inventar destinos
    comprobar(afn.destinos(1, 'a').empty(), "Transicion ausente incorrecta");
    comprobar(afn.destinos(0, 'b').empty(), "Simbolos mezclados");
    comprobar(afn.destinosEpsilon(1).empty(), "Epsilon ausente incorrecto");
    comprobar(afn.transiciones == transicionesAntes, "Consultar modifica las tablas");
    // Los IDs negativos no son validos para registrar estados
    esperarExcepcion<std::invalid_argument>([&] { afn.agregarTransicion(-1, 'a', 0); });
    esperarExcepcion<std::invalid_argument>([&] { afn.agregarEpsilon(0, -1); });

    // Los IDs del AFN no tienen que ser consecutivos ni comenzar en cero
    automata::AFN otro;
    otro.agregarTransicion(5, 'b', 9);
    comprobar(otro.alfabeto == std::set<char>({'b'}), "No se registro el simbolo");
    comprobar(otro.estados == automata::ConjuntoEstados({5, 9}), "IDs no consecutivos incorrectos");
}

// Verifica subconjuntos, IDs y la restriccion de un destino por simbolo
void probarAFD() {
    automata::AFD afd;
    // agregarEstado devuelve un ID del AFD, no un estado individual del AFN
    int inicio = afd.agregarEstado({2, 0, 1});
    int fin = afd.agregarEstado({3});
    // El conjunto vacio puede representar un pozo, su bucle se agrega mas abajo
    int pozo = afd.agregarEstado({});
    // El orden de los elementos no cambia un conjunto: se debe reutilizar el ID
    comprobar(afd.agregarEstado({1, 2, 0}) == inicio, "Subconjunto duplicado");
    comprobar(afd.estados.size() == 3, "Cantidad de estados AFD incorrecta");
    comprobar(afd.estados[inicio] == automata::ConjuntoEstados({0, 1, 2}), "Correspondencia incorrecta");
    comprobar(afd.buscarEstado({}) == pozo, "No se reconoce el conjunto vacio");
    // -1 indica que no se encontro el subconjunto, 0 si es un ID valido
    comprobar(afd.buscarEstado({99}) == -1, "Se encontro un subconjunto inexistente");
    afd.inicial = inicio;
    afd.finales = {inicio, fin}; 
    afd.agregarTransicion(inicio, 'a', fin);
    afd.agregarTransicion(inicio, 'a', fin); 
    // Cambiarlo por otro destino debe fallar y conservar la transicion original
    esperarExcepcion<std::invalid_argument>([&] { afd.agregarTransicion(inicio, 'a', pozo); });
    comprobar(afd.destino(inicio, 'a') == fin, "Se sobrescribio una transicion AFD");
    comprobar(afd.destino(fin, 'a') == -1, "No se admite AFD parcial");
    // Con alfabeto {'a'}, este bucle hace que el pozo permanezca en si mismo
    afd.agregarTransicion(pozo, 'a', pozo);
    comprobar(afd.destino(pozo, 'a') == pozo, "Bucle del pozo incorrecto");
    comprobar(afd.alfabeto == std::set<char>({'a'}), "No se registro el simbolo AFD");
    // Rechaza tanto IDs positivos inexistentes como IDs negativos
    esperarExcepcion<std::invalid_argument>([&] { afd.agregarTransicion(inicio, 'a', 99); });
    esperarExcepcion<std::invalid_argument>([&] { afd.agregarTransicion(-1, 'a', fin); });
}

// Revisa elementos clave del texto impreso.
void probarTablas() {
    automata::AFN afn;
    afn.estados = {0};
    afn.finales = {0};
    afn.alfabeto = {'a'};
    afn.inicial = 0;
    std::ostringstream salidaAFN;
    afn.imprimirTabla(salidaAFN);
    comprobar(salidaAFN.str().find("->*0") != std::string::npos, "Faltan marcas inicial/final");
    comprobar(salidaAFN.str().find("epsilon") != std::string::npos, "Falta columna epsilon");

    // El AFD debe mostrar su subconjunto del AFN, pero no una columna epsilon
    automata::AFD afd;
    afd.alfabeto = {'a'};
    afd.inicial = afd.agregarEstado({0, 1});
    std::ostringstream salidaAFD;
    afd.imprimirTabla(salidaAFD);
    comprobar(salidaAFD.str().find("{0,1}") != std::string::npos, "Falta subconjunto en tabla");
    comprobar(salidaAFD.str().find("epsilon") == std::string::npos, "AFD muestra epsilon");

    // AFN{} y AFD{} crean automatas vacios temporales, aun sin estado inicial
    std::ostringstream salidaVacia;
    automata::AFN{}.imprimirTabla(salidaVacia);
    automata::AFD{}.imprimirTabla(salidaVacia);
    comprobar(salidaVacia.str().find("sin definir") != std::string::npos, "Inicial ausente no indicado");
}

}

int main() {
    try {
        // Ejecuta los tres grupos en orden.
        probarAFN();
        probarAFD();
        probarTablas();
        // Solo se llega aqui si todas las comprobaciones anteriores pasaron.
        std::cout << "OK: estructuras AFN/AFD, validaciones y tablas\n";
    } catch (const std::exception& error) {
        // what() devuelve el mensaje de la excepcion que provoco el fallo.
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1; 
    }
    // Llegar al final de main equivale a return 0: ejecucion exitosa.
}
