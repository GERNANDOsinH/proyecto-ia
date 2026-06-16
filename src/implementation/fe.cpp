#include "algorithm.hpp"

using namespace std;

uint AE::FE(uint i) {
    uint curr_fe = 0;
    uint power;
    for (uint j = 0;j < num_nodes;j++){
        power = (poblacion[i][j])? nodes[j].f_i : 0;
        curr_fe += (poblacion[i][j])? nodes[i].C_i : 0;
        for (uint k = 0;k < num_nodes;k++) {
            if (j == k)
                continue;
            if (!poblacion[i][k])
                continue;
            if (nodes[k].S_i*distancias[j][k] <= R)
                power += nodes[k].f_i;
        }
        if (power < nodes[j].D_i)
            curr_fe += penalty_2;
    }
    vector<vector<bool>> alcanzable(num_nodes, vector<bool>(num_nodes, false));
    for (uint u = 0; u < num_nodes; u++) {
        for (uint v = 0; v < num_nodes; v++) {
            if (u == v || distancias[u][v] <= R) {
                alcanzable[u][v] = true;
            }
        }
    }
    for (uint k = 0; k < num_nodes; k++) {
        if (!poblacion[i][k]) continue;

        for (uint u = 0; u < num_nodes; u++) {
            for (uint v = 0; v < num_nodes; v++) {
                if (alcanzable[u][k] && alcanzable[k][v]) {
                    alcanzable[u][v] = true;
                }
            }
        }
    }
    uint rutas_invalidas = 0;
    for (uint u = 0; u < num_nodes; u++) {
        for (uint v = 0; v < num_nodes; v++) {
            if (!alcanzable[u][v]) {
                rutas_invalidas++;
            }
        }
    }
    curr_fe += rutas_invalidas * penalty_1;

    return curr_fe;
}