#include "algorithm.hpp"

using namespace std;

void AE::print(uint i) {
    for (uint j = 0; j < num_nodes; j++)
        cout << ((poblacion[i][j])? "1 " : "0 ");
    cout << "FE: " << FE(i) << endl;
}

void AE::print() {
    if (size_poblation == 0) {
        cout << "La poblacion esta vacia." << endl;
        return;
    }

    uint best_idx = 0;
    
    // Aseguramos que el primer individuo este reparado antes de evaluar su FE real
    repair(poblacion[0]);
    uint min_fe = FE(0);

    // Recorremos el resto de la población buscando el FE mínimo (el mejor)
    for (uint i = 1; i < size_poblation; i++) {
        uint current_fe = FE(i);
        
        if (current_fe < min_fe) {
            min_fe = current_fe;
            best_idx = i;
        }
    }

    // Imprimimos el mejor individuo encontrado
    cout << "=== MEJOR INDIVIDUO ENCONTRADO ===" << endl;
    cout << "Indice en la poblacion: " << best_idx << endl;
    print(best_idx); // Reutiliza el método que imprime el cromosoma y su FE
    cout << "==================================" << endl;
}