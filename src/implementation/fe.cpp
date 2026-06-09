#include "algorithm.hpp"

using namespace std;

uint AE::FE(uint i) {
    uint curr_fe = 0;
    for (uint j = 0;j < num_nodes;j++)
        curr_fe += (poblacion[i][j])? nodes[i].C_i : 0;

    return curr_fe;
}