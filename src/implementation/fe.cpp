#include "algorithm.hpp"

using namespace std;

uint AE::FE(uint i) {
    uint curr_fe = 0;
    
    // Cálculo de Costo Real (Corrigiendo el bug de indices nodes[i] a nodes[j])
    for (uint j = 0; j < num_nodes; j++) {
        if (poblacion[i][j]) {
            curr_fe += nodes[j].C_i; 
        }
    }

    // Construcción de la matriz de alcanzabilidad 
    std::vector<std::vector<bool>> alcanzable(num_nodes, std::vector<bool>(num_nodes, false));
    for (uint u = 0; u < num_nodes; u++) {
        for (uint v = 0; v < num_nodes; v++) {
            if (u == v || distancias[u][v] <= R) alcanzable[u][v] = true;
        }
    }
    for (uint k = 0; k < num_nodes; k++) {
        if (!poblacion[i][k]) continue;
        for (uint u = 0; u < num_nodes; u++) {
            for (uint v = 0; v < num_nodes; v++) {
                if (alcanzable[u][k] && alcanzable[k][v]) alcanzable[u][v] = true;
            }
        }
    }

    // Cálculo de rutas inválidas (Se mantiene penalty_1 por requerir validación global del subgrafo)
    uint rutas_invalidas = 0;
    for (uint u = 0; u < num_nodes; u++) {
        for (uint v = 0; v < num_nodes; v++) {
            if (!alcanzable[u][v]) rutas_invalidas++;
        }
    }
    curr_fe += rutas_invalidas * penalty_1;

    return curr_fe;
}