<div style="text-align: left;">
  <img width="200" src="https://www.fciencias.unam.mx/sites/default/files/logoFC_2.png" alt="Logo FC">
</div>

# Practica 2: AFN a AFD

## Compiladores 2027-1
En este repositorio se implementa el algoritmo de **construcción de subconjuntos** para transformar un Autómata Finito No Determinista (**NFA**) en un Autómata Finito Determinista (**DFA**) equivalente.

La implementación utiliza las operaciones fundamentales:

- `move(T, a)`: obtiene los estados alcanzables desde un conjunto de estados `T` consumiendo el símbolo `a`.
- `ε-closure(T)`: obtiene todos los estados alcanzables desde `T` mediante cero o más transiciones ε.
- `subconjuntos(NFA)`: construye el DFA equivalente utilizando `move` y `ε-closure`.

**Alumnos:**

| Nombre Completo                 | Número de cuenta |
|---------------------------------| ---------------- |
| Chávez Martínez Marco Antonio   | ---------        |
| Lugo Díaz Ordaz Gretel Alexandra| ---------        |
| Hernández Islas Leonardo Daniel | ---------        |
| Vega Alonso Diego Hazael        | 321301183        |
