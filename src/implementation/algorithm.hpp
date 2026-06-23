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
};

class AE {
    protected:
        // === Atributos algoritmo === //
        uint size_poblation;
        uint penalty_1; // Penalización no transitabilidad de los nodos.
        uint penalty_2; // Penalización demanda no satisfecha.
        uint penalty_3; // Penalización punto remoto no usado.
        double beta;    // Probabilidad de mutación.
        std::mt19937 rng;

        std::vector<std::vector<bool>> poblacion;
        // === Atributos problema === //
        uint max_cost;
        uint CityArea;
        uint R;
        uint num_nodes;
        double alpha;


        std::vector<node> nodes;
        std::vector<std::vector<double>> distancias;

        // === Métodos protegidos === //
        virtual std::vector<std::vector<bool>> cruzamiento(uint i, uint j);
        virtual void mutacion(uint i);
        virtual void iteration();
    public:
        AE(uint size_poblation, uint P1, uint P2, uint P3, double beta, std::string src);
        ~AE() = default;
        uint FE(uint i);
        void print();
        void print(uint i);
        void solve(uint max_iterations);
        void repair();
        void repair(uint i);
};

#endif;