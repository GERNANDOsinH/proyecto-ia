#include "algorithm.hpp"
#include <vector>
#include <random>

using namespace std;

void AE::iteration() {
    uint curr_fe;
    vector<double> FEs(num_nodes);
    double total = 0.0;
    double better = 0.0;
    uint better_index = 0;

    for (uint i = 0;i < size_poblation;i++) {
        curr_fe = FE(i);
        FEs[i] = 1.0 - (double)curr_fe/max_cost;
        total += FEs[i];
        if (better < FEs[i]) {
            better = FEs[i];
            better_index = i;
        }
    }
    for (uint i = 0;i < size_poblation;i++)
        FEs[i] /= total;
    
    poblacion[0].swap(poblacion[better_index]);

    vector<uint> padres;
    padres.push_back(0);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0-FEs[0]);
    double generado;

    while (padres.size() < size_poblation/2) {
        generado = dist(gen);
        total = 0.0;
        for (uint i = 1;i < size_poblation;i++) {
            total += FEs[i];
            if (generado <= total) {
                padres.push_back(i);
                break;
            }
        }
    }
    vector<vector<bool>> nueva_poblacion;
    nueva_poblacion.push_back(poblacion[0]);
    uniform_int_distribution<uint> dist_padres(0, padres.size() - 1);

while (nueva_poblacion.size() < size_poblation) {
        uint idx_p1 = padres[dist_padres(gen)];
        uint idx_p2 = padres[dist_padres(gen)];
        
        while (idx_p1 == idx_p2 && padres.size() > 1) {
            idx_p2 = padres[dist_padres(gen)];
        }

        vector<vector<bool>> hijos = cruzamiento(idx_p1, idx_p2);

        for (size_t i = 0; i < hijos.size(); i++) {
            if (nueva_poblacion.size() < size_poblation) {
                nueva_poblacion.push_back(hijos[i]);
            }
        }
    }

    poblacion = nueva_poblacion;
    for (uint i = 1; i < size_poblation; i++)
        mutacion(i);

}