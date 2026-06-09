#include <pybind11/pybind11.h>
#include "utils.hpp"

namespace py = pybind11;

PYBIND11_MODULE(ae_modulo, m) {
    m.doc() = "Módulo de Algoritmo Evolutivo usando pybind11";

    // 1. Exponer la estructura 'results' para poder leerla en Python
    py::class_<results>(m, "Results")
        .def_readwrite("better_FE", &results::better_FE)
        .def_readwrite("average_FE", &results::average_FE);

    // 2. Exponer la clase base 'AE'
    py::class_<AE>(m, "AE")
        // Exponer el constructor
        .def(py::init<uint64_t, double, std::string>(), 
             py::arg("size_poblation"), py::arg("beta"), py::arg("src"))
        // Exponer los métodos públicos
        .def("repair", &AE::repair, py::arg("i"))
        .def("print", &AE::print)
        .def("FE", &AE::FE, py::arg("i"))
        .def("solve", &AE::solve, py::arg("max_iterations"));

    // 3. Exponer la clase derivada 'uniform_AE'
    // Nota: Pasamos 'AE' como segundo parámetro del template para indicar herencia
    py::class_<uniform_AE, AE>(m, "Uniform_AE")
        .def(py::init<uint64_t, double, std::string>(), 
             py::arg("size_poblation"), py::arg("beta"), py::arg("src"));
}