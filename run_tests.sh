#!/bin/bash

echo "⚙️  Compilando el proyecto..."
make clean
make

if [ $? -ne 0 ]; then
    echo "❌ Error en la compilación. Revisa el código."
    exit 1
fi

echo -e "\n==================================="
echo "Ejecutando pruebas de Autómata..."
./prueba_automata

echo -e "\n==================================="
echo "Ejecutando pruebas de Operaciones..."
./prueba_operations

echo -e "\n==================================="
echo "Ejecutando Validación Integral (NFA a DFA)..."
./test_conversion

echo -e "\n==================================="
echo "✅ ¡Todas las pruebas finalizaron!"