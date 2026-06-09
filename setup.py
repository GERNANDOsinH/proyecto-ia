from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "ae_modulo",                             
        # Actualizamos las rutas hacia la nueva estructura
        ["src/cpp/bindings/bindings.cpp", "src/cpp/core/utils.cpp"],           
        extra_compile_args=["-std=c++11", "-O3"] 
    ),
]

setup(
    name="ae_modulo",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)