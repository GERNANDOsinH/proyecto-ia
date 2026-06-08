#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdint>

struct node {
    bool is_far; // Declara si el nodo es lejano o no.
    uint64_t f_i;    // Capacidad de carga del nodo(si se coloca un cargador).
    uint64_t c_i;    // Costo asociado a colocar un punto de carga en el nodo.
    uint64_t D_i;    // Demanda del nodo i.
    uint64_t s_i;    // Rango del nodo i.
};

struct results {
    uint64_t better_FE;
    uint64_t average_FE;
};

struct option {
    int index;
    double coef;
};

class AE {
    protected:
        // ==== Atributos algoritmo ==== // 
        double beta;                           // Probabilidad de una mutación.
        std::mt19937 motor;                    // Generador de números aleatorios.
        uint64_t size_poblation;               // Tamaño de la población.
        std::uniform_real_distribution<double> dist_double;

        // ==== Atributos problema ==== //
        double alpha;
        uint64_t CityArea;                        // Área de la ciudad que se desea cubrir.
        uint64_t R;                               // Autonomia promedio de los vehiculos.
        uint64_t num_nodes;                       // Número de nodos disponibles, incluyendo nodos lejanos.
   
        std::vector<node> nodes;                  // Nodos.
        std::vector<std::vector<uint8_t>> matrix; // Matriz población.
        std::vector<double> FEs;                  // Función de evaluación de cada instancia en la población.
        std::vector<std::vector<double>> dist;    // Distancia entre todos los nodos.

        // === Método protegido === //
        virtual void cruce(int i, int j, int i_p, int j_p, std::vector<std::vector<uint8_t>>& next_gen);
    public:
        AE(uint64_t size_poblation, double beta, std::string src);
        bool repair(int i);
        results print();
        virtual ~AE();
        virtual uint64_t FE(int i);
        virtual void solve(uint64_t max_iterations);
};

class uniform_AE : public AE {
    protected:
        void cruce(int i, int j, int i_p, int j_p, std::vector<std::vector<uint8_t>>& next_gen) override;
    public:
        uniform_AE(uint64_t size_poblation, double beta, std::string src) : AE(size_poblation, beta, src) {};
};

#endif;