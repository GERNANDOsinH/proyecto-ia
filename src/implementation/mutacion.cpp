#include "algorithm.hpp"
#include <random>

using namespace std;

void AE::mutacion(uint i) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(0, 1);
    double generado;

    for (uint j = 0;j < num_nodes;j++) {
        generado = dist(gen);
        if (generado <= beta)
            poblacion[i][j] = !poblacion[i][j];
    }
}