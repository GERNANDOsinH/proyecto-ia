#include "algorithm.hpp"

using namespace std;

void AE::print(uint i) {
    for (uint j = 0; j < num_nodes; j++)
        cout << (poblacion[i][j])? "1 " : "0 ";
    cout << "FE: " << FE(i) << endl;
}

void AE::print() {
    for (uint i = 0; i < size_poblation; i++)
        print(i);
}