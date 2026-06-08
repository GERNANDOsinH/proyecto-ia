#include <iostream>
#include <string>
#include "utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    int max_iterations;
    AE* solver = nullptr;
    if (argc == 5) {
        int n = stoi(argv[1]);
        float beta = stof(argv[2]);
        string src = argv[3];
        solver = new AE(n, beta, src);
        max_iterations = stoi(argv[4]);
    }
    else if (argc == 8) {
        int n = stoi(argv[1]);
        float beta = stof(argv[2]);
        string src = argv[3];
        uint M1 = (uint)stoi(argv[4]);
        uint M2 = (uint)stoi(argv[5]);
        uint M3 = (uint)stoi(argv[6]);
        max_iterations = stoi(argv[7]);

        solver = new penalty_AE(n, beta, M1, M2, M3, src);
    }
    else {
        cout << "Número de argumentos inválido";
        return 1;
    }
    solver->save("first_poblation.txt");
    solver->solve(max_iterations);
    solver->save("results.txt");
    
    return 0;
}