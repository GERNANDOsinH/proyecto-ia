#include "algorithm.hpp"

using namespace std;

void AE::solve(uint max_iterations) {
    uint iterations_for_control = 100;
    uint penalty_change = max_cost*(double)iterations_for_control/max_iterations;

    for (uint i = 0;i < max_iterations;i++) {
        if (i % iterations_for_control) {
            penalty_1 += penalty_change;
            penalty_2 += penalty_change;
        }
        iteration();
    }
}