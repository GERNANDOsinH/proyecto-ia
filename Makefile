# 1. Variables de configuración
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -Isrc/implementation

# 2. Nombre ejecutable final
TARGET = ae_solver

# 3. Lista de archivos de objeto (.o) que queremos generar
OBJS = obj/main.o \
       obj/constructor.o \
       obj/fe.o \
       obj/print.o


# === Reglas de compilación === #

# Regla por defecto (la que se ejecuta al solo escribir 'make')
all: $(TARGET)

# Regla para enlazar todos los archivos .o y crear un ejecutable.
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regla específica para compilar src/main.cpp
obj/main.o: src/main.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla genérica para compilar los archivos de src/implementation/
obj/%.o: src/implementation/%.cpp src/implementation/algorithm.hpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados y dejar el proyecto limpio.
clean:
	rm -rf obj $(TARGET)

# Indica a Make que estas reglas no representan archivos reales
.PHONY: all clean