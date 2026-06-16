#include "algorithm.hpp"

using namespace std;

void AE::solve(uint max_iterations) {
    this->penalty_1 = max_cost;
    this->penalty_2 = max_cost;

    for (uint i = 0;i < max_iterations;i++)
        iteration();
    
}