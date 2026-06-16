#include "algorithm.hpp"

using namespace std;

uint AE::FE(uint i) {
    uint curr_fe = 0;
    uint power;
    for (uint j = 0;j < num_nodes;j++){
        power = nodes[j].f_i;
        curr_fe += (poblacion[i][j])? nodes[i].C_i : 0;
        for (uint k = 0;k < num_nodes;k++) {
            if (j == k)
                continue;
            if (nodes[k].S_i*distancias[j][k] <= R)
                power += nodes[k].f_i;
        }
        if (power < nodes[j].D_i)
            curr_fe += penalty_2;
    }

    // Simulación

    return curr_fe;
}