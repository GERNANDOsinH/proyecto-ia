#include "algorithm.hpp"

using namespace std;

void AE::solve(uint max_iterations) {
    for (uint i = 0;i < max_iterations;i++)
        iteration();
}