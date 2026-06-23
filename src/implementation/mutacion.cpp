#include "algorithm.hpp"
#include <random>

using namespace std;

void AE::mutacion(uint i) {
    uniform_real_distribution<double> dist(0, 1);
    double generado;

    for (uint j = 0;j < num_nodes;j++) {
        generado = dist(rng);
        if (generado <= beta)
            poblacion[i][j] = !poblacion[i][j];
    }
}