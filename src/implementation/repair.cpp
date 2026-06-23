#include "algorithm.hpp"
#include <vector>

using namespace std;

void AE::repair(vector<bool>& individuo) {
    // --- FASE 1: Reparación Geográfica O(1) ---
    // Si el algoritmo intentó poner un cargador en un lago/edificio, lo movemos
    for (uint i = 0; i < num_nodes; i++) {
        if (individuo[i] == true && nearest_feasible_node[i] != i) {
            individuo[i] = false; // Quitamos del nodo inválido
            uint valid_pos = nearest_feasible_node[i];
            individuo[valid_pos] = true; // Movemos al válido más cercano
        }
    }

    // --- FASE 2: Reparación Funcional ---
    
    // 2.1 Reparación de Nodo Remoto
    bool far_is_covered = false;
    uint far_node_id = 0;
    for (uint j = 0; j < num_nodes; j++) {
        if (nodes[j].is_far) far_node_id = j;
        if (individuo[j] && nodes[j].is_far) far_is_covered = true;
    }
    if (!far_is_covered) {
        // Nos aseguramos que el nodo forzado sea uno válido geográficamente
        individuo[nearest_feasible_node[far_node_id]] = true; 
    }

    // 2.2 Reparación de Demanda Satisfecha
    for (uint j = 0; j < num_nodes; j++) {
        uint power = (individuo[j]) ? nodes[j].f_i : 0;
        for (uint k = 0; k < num_nodes; k++) {
            if (j == k || !individuo[k]) continue;
            if (nodes[k].S_i * distancias[j][k] <= R) {
                power += nodes[k].f_i;
            }
        }
        
        // Si la potencia no alcanza, activamos el nodo j 
        // (o su vecino válido más cercano si j es un nodo geográficamente inválido)
        if (power < nodes[j].D_i) {
            individuo[nearest_feasible_node[j]] = true; 
        }
    }
}