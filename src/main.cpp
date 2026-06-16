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

    cout << "==== POBLACIÓN INICIAL ====" << endl;

    AE solver(size_poblation, beta, src);
    solver.print();
    solver.solve(1000000);

    cout << "==== POBLACIÓN FINAL ====" << endl;
    solver.print();

    return 0;
}