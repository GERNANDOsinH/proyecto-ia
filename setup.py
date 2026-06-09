from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "ae_modulo",                             # Nombre del módulo en Python
        ["bindings.cpp", "utils.cpp"],           # Archivos fuente a compilar
        extra_compile_args=["-std=c++11", "-O3"] # Opciones extra (ej. optimización)
    ),
]

setup(
    name="ae_modulo",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)