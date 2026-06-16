TARGET = ae_solver

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(SRC_DIR)/main.cpp $(wildcard $(SRC_DIR)/implementation/*.cpp)

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Ejecutable '$@' creado con éxito."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/* $(TARGET)
	@echo "Limpieza completada."

.PHONY: all clean