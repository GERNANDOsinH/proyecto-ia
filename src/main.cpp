#include "implementation/algorithm.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cout << "Número de parámtros entregados incorrectos";
        return 1;
    }    
    uint size_poblation = stoull(argv[1]);
    uint P1 = stoull(argv[2]);
    uint max_iterations = stoull(argv[3]);
    string src = argv[4];

    cout << "==== POBLACIÓN INICIAL ====" << endl;

    AE solver(size_poblation, P1, src);
    auto aux = AE(solver);
    aux.print();

    solver.solve(max_iterations);

    cout << "==== POBLACIÓN FINAL ====" << endl;
    solver.print();

    return 0;
}