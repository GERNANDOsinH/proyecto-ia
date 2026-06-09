# ==============================================================================
# Makefile para Proyecto AE (Algoritmo Evolutivo Híbrido C++/Python)
# ==============================================================================

# Configuración del entorno y ejecutables
PYTHON      = python3
PIP         = pip3
SETUP_SCRIPT = setup.py
SRC_DIR     = src
BUILD_DIR   = build

# Detectar el sistema operativo para la extensión del binario
ifeq ($(OS),Windows_NT)
    RM = del /Q /F /S
    RMDIR = rmdir /S /Q
    EXTENSION = *.pyd
else
    RM = rm -f
    RMDIR = rm -rf
    EXTENSION = *.so
endif

.PHONY: all venv build run clean help

# Objetivo por defecto
all: build

help:
	@echo "Comandos disponibles:"
	@echo "  make venv    - Crea el entorno virtual e instala dependencias"
	@echo "  make build   - Compila la extensión C++ (pybind11) in-place"
	@echo "  make run     - Ejecuta la aplicación principal en Python"
	@echo "  make clean   - Elimina archivos temporales de compilación y caché"

# Crear entorno virtual e instalar dependencias necesarias
venv:
	@echo "Creando entorno virtual..."
	$(PYTHON) -m venv .venv
	@echo "Instalando dependencias de compilación..."
	.venv/bin/pip install --upgrade pip setuptools wheel pybind11

# Compilar el módulo C++ usando setup.py
build:
	@echo "Compilando extensión C++ mediante pybind11..."
	if [ -d ".venv" ]; then \
		.venv/bin/python $(SETUP_SCRIPT) build_ext --inplace; \
	else \
		$(PYTHON) $(SETUP_SCRIPT) build_ext --inplace; \
	fi
	@echo "Compilación completada con éxito."

# Ejecutar el script principal de Python
run:
	@echo "Ejecutando la aplicación principal..."
	if [ -d ".venv" ]; then \
		PYTHONPATH=. .venv/bin/python $(SRC_DIR)/main.py; \
	else \
		PYTHONPATH=. $(PYTHON) $(SRC_DIR)/main.py; \
	fi

# Limpieza completa del proyecto
clean:
	@echo "Limpiando artefactos de compilación..."
	$(RMDIR) $(BUILD_DIR)
	$(RMDIR) .eggs
	$(RMDIR) *.egg-info
	$(RM) $(EXTENSION)
	@echo "Limpiando caché de Python..."
	find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@echo "Limpieza finalizada."