# Opciones del compilador
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O3

# Archivos del proyecto
SRCS = main.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = solver

# Regla principal: compila, enlaza y luego borra los .o
all: $(TARGET)
	@echo "Compilación exitosa. Eliminando archivos intermedios (.o)..."
	@rm -f $(OBJS)

# Regla para generar el ejecutable final
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regla para generar los archivos objeto intermedios
%.o: %.cpp utils.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla tradicional para limpiar todo (incluyendo el ejecutable)
clean:
	@echo "Limpiando el directorio..."
	@rm -f $(TARGET) $(OBJS)