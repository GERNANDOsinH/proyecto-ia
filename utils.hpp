#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

#define uint unsigned int

using namespace std;

struct node {
    bool is_far; // Declara si el nodo es lejano o no.
    uint f_i;    // Capacidad de carga del nodo(si se coloca un cargador).
    uint c_i;    // Costo asociado a colocar un punto de carga en el nodo.
    uint D_i;    // Demanda del nodo i.
    uint s_i;    // Rango del nodo i.
};

struct option {
    int index;
    float coef;
};

class AE {
    protected:
        // ==== Atributos algoritmo ==== // 
        float beta;                  // Probabilidad de una mutación.
        mt19937 motor;               // Generador de números aleatorios.
        uint size_poblation;         // Tamaño de la población.
        uniform_int_distribution<int> dist_int;
        uniform_real_distribution<float> dist_float;

        // ==== Atributos problema ==== //
        float alpha;
        uint CityArea;               // Área de la ciudad que se desea cubrir.
        uint R;                      // Autonomia promedio de los vehiculos.
        uint num_nodes;              // Número de nodos disponibles, incluyendo nodos lejanos.

        vector<node> nodes;          // Nodos.
        vector<vector<bool>> matrix; // Matriz población.
        vector<float> FEs;            // Función de evaluación de cada instancia en la población.
        vector<vector<float>> dist;  // Distancia entre todos los nodos.

        // === Método protegido === //
        void cruce(int pos,int i, int j, int i_p, int j_p);
    public:
        AE(uint size_poblation, float beta, string src);
        ~AE();
        virtual uint FE(int i);
        bool repair(int i);
        virtual void solve(uint max_iterations);
};

class penalty_AE : public AE {
    protected:
        uint M1; // Penalización no asignación nodos lejanos.
        uint M2; // Penalización no transitividad.
        uint M3; // Penalización demandas no cubiertas.
    public:
        penalty_AE(uint size_poblation, float beta, uint M1, uint M2, uint M3, string src);
        ~penalty_AE();
        uint FE(int i) override;
        void solve(uint max_iterations) override;
};

#endif