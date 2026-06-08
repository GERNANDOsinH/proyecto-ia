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

struct option {
    int index;
    double coef;
};

class AE {
    protected:
        // ==== Atributos algoritmo ==== // 
        double beta;                            // Probabilidad de una mutación.
        std::mt19937 motor;                    // Generador de números aleatorios.
        uint64_t size_poblation;                   // Tamaño de la población.
        std::uniform_real_distribution<double> dist_double;

        // ==== Atributos problema ==== //
        double alpha;
        uint64_t CityArea;                            // Área de la ciudad que se desea cubrir.
        uint64_t R;                                   // Autonomia promedio de los vehiculos.
        uint64_t num_nodes;                           // Número de nodos disponibles, incluyendo nodos lejanos.
   
        std::vector<node> nodes;                  // Nodos.
        std::vector<std::vector<uint8_t>> matrix; // Matriz población.
        std::vector<double> FEs;                   // Función de evaluación de cada instancia en la población.
        std::vector<std::vector<double>> dist;     // Distancia entre todos los nodos.

        // === Método protegido === //
        void cruce(int i, int j, int i_p, int j_p, std::vector<std::vector<uint8_t>>& next_gen);
    public:
        AE(uint64_t size_poblation, double beta, std::string src);
        virtual ~AE();
        virtual uint64_t FE(int i);
        bool repair(int i);
        virtual void solve(uint64_t max_iterations);
        void save(std::string src);
};

class penalty_AE : public AE {
    protected:
        uint64_t M1; // Penalización no asignación nodos lejanos.
        uint64_t M2; // Penalización no transitividad.
        uint64_t M3; // Penalización demandas no cubiertas.
    public:
        penalty_AE(uint64_t size_poblation, double beta, uint64_t M1, uint64_t M2, uint64_t M3, std::string src);
        virtual ~penalty_AE();
        uint64_t FE(int i, uint64_t& out_base_cost);
        uint64_t FE(int i) override;
        void solve(uint64_t max_iterations) override;
};

#endif;