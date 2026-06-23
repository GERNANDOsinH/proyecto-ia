#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <random>

struct node {
    uint C_i;    // Costo de instalar un cargador en el nodo i.
    uint S_i;    // Rango de servicio del nodo i.
    uint D_i;    // Demenda en el nodo i.
    uint f_i;    // Poder de carga del nodo i.
    bool is_far; // Es el nodo remoto.
    double x, y; // Punto en el que se ubica el nodo i.
};

class AE {
    protected:
        // === Atributos algoritmo === //
        uint size_poblation; // Tamaño población.
        double beta;         // Probabilidad de mutación.
        uint penalty_1;      // Penalización no transitivilidad.
        std::mt19937 rng;    // Motor generador de números aleatorios.

        std::vector<std::vector<bool>> poblacion;
        std::vector<uint> nearest_feasible_node; 
        // === Atributos problema === //
        uint max_cost;
        uint CityArea;
        uint R;
        uint num_nodes;
        double alpha;


        std::vector<node> nodes;
        std::vector<std::vector<double>> distancias;

        // === Métodos protegidos === //
        std::vector<std::vector<bool>> cruzamiento(uint i, uint j);
        void mutacion(uint i);
        void iteration();
        void precalculate_feasibility();
        void repair(std::vector<bool>& individuo);
    public:
        AE(uint size_poblation, uint P1, std::string src);
        ~AE() = default;
        uint FE(uint i);
        void print();
        void print(uint i);
        void solve(uint max_iterations);
};

#endif