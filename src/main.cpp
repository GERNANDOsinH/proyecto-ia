#include "implementation/algorithm.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 7) {
        cout << "Número de parámtros entregados incorrectos";
        return 1;
    }    
    uint size_poblation = stoull(argv[1]);
    uint P1 = stoull(argv[2]);
    uint P2 = stoull(argv[3]);
    uint P3 = stoull(argv[4]);
    double beta = stod(argv[5]);
    string src = argv[6];

    cout << "==== POBLACIÓN INICIAL ====" << endl;

    AE solver(size_poblation, P1, P2, P3, beta, src);
    auto aux = AE(solver);
    aux.repair();
    aux.print();

    solver.solve(1000000);

    cout << "==== POBLACIÓN FINAL ====" << endl;
    solver.repair();
    solver.print();

    return 0;
}