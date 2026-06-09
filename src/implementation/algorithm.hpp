#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

typedef uint uint64_t;

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
        double beta;

        std::vector<std::vector<bool>> poblacion;
        // === Atributos problema === //
        uint CityArea;
        uint R;
        uint num_nodes;
        double alpha;


        std::vector<node> nodes;
        std::vector<std::vector<double>> distancias;

        // === Métodos protegidos === //
        virtual std::vector<bool> cruzamiento(uint i, uint j) {};
        virtual std::vector<bool> mutacion(uint i) {};
        virtual void iteration() {};
    public:
        AE(uint size_poblation, double beta, std::string src);
        ~AE() = default;
        uint FE(uint i);
        void print();
        void print(uint i);
        void solve(uint max_iterations) {};
};

#endif