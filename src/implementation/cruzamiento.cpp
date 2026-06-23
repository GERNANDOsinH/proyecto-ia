#include "algorithm.hpp"
#include <vector>
#include <random>

using namespace std;

vector<vector<bool>> AE::cruzamiento(uint i, uint j) {
    uniform_int_distribution<uint> dist(0, num_nodes-1);
    uint punto = dist(rng);

    vector<vector<bool>> ret(2, vector<bool>(num_nodes));

    for (uint k = 0;k < num_nodes;k++) {
        if (k < punto) {
            ret[0][k] = poblacion[i][k];
            ret[1][k] = poblacion[j][k];
        }
        else {
            ret[0][k] = poblacion[j][k];
            ret[1][k] = poblacion[i][k];
        }
    }
    return ret;
}