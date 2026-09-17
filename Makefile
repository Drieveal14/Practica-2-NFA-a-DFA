CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

# Ejecutables a generar
TARGETS = main prueba_automata prueba_operations test_conversion

all: $(TARGETS)

main: main.cpp automata.cpp operations.cpp converter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

prueba_automata: pruebas/prueba_automata.cpp automata.cpp operations.cpp converter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

prueba_operations: pruebas/prueba_operations.cpp automata.cpp operations.cpp converter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_conversion: pruebas/test_conversion.cpp automata.cpp operations.cpp converter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGETS)