#include "algorithm.hpp"
#include <limits>

using namespace std;

void AE::precalculate_feasibility() {
    nearest_feasible_node.resize(num_nodes);

    for (uint i = 0; i < num_nodes; i++) {
        // Definimos la regla de qué hace a un nodo "geográfica/técnicamente" válido.
        // Asumimos que es válido si tiene potencia de carga mayor a 0 y un costo razonable.
        // Ajusta el '99999' según cómo representes un nodo intransitable en tu txt.
        bool es_nodo_valido = (nodes[i].f_i > 0 && nodes[i].C_i < 99999); 

        if (es_nodo_valido) {
            nearest_feasible_node[i] = i; // Es válido por sí mismo
        } else {
            double min_dist = numeric_limits<double>::max();
            uint closest_id = i;

            // Buscamos el nodo válido más cercano
            for (uint j = 0; j < num_nodes; j++) {
                bool j_es_valido = (nodes[j].f_i > 0 && nodes[j].C_i < 99999); 
                
                if (j_es_valido && distancias[i][j] < min_dist) {
                    min_dist = distancias[i][j];
                    closest_id = j;
                }
            }
            nearest_feasible_node[i] = closest_id;
        }
    }
}