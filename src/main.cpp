#include "implementation/algorithm.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Número de parámtros entregados incorrectos";
        return 1;
    }    
    uint size_poblation = stoull(argv[1]);
    double beta = stod(argv[2]);
    string src = argv[3];

    AE solver(size_poblation, beta, src); // Inicializa las soluciones iniciales.
    solver.print();
    return 0;
}