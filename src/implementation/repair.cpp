#include "algorithm.hpp"
#include <vector>
#include <algorithm>

void AE::repair(uint i) {
    // Obtenemos la referencia a la solución actual (individuo i)
    std::vector<bool>& sol = poblacion[i];

    // =================================================================
    // FASE 1: Reparación de restricciones absolutas (Puntos Remotos)
    // Aseguramos que los nodos remotos obligatorios estén encendidos
    // =================================================================
    for (uint j = 0; j < num_nodes; j++) {
        if (nodes[j].is_far && !sol[j]) {
            sol[j] = true; 
        }
    }

    // Vector temporal para almacenar la potencia total que recibe cada nodo
    std::vector<uint> demanda_cubierta(num_nodes, 0);

    // Función lambda para recalcular la cobertura actual de la red
    auto recalcular_cobertura = [&]() {
        std::fill(demanda_cubierta.begin(), demanda_cubierta.end(), 0);
        for (uint j = 0; j < num_nodes; j++) {
            if (sol[j]) { // Si hay una estación instalada en j
                for (uint k = 0; k < num_nodes; k++) {
                    // Verificamos si el nodo k está dentro del rango de servicio (S_i o R)
                    // Se asume S_i como rango local, pero puede ajustarse a distancias[j][k] <= R
                    if (distancias[j][k] <= nodes[j].S_i) { 
                        demanda_cubierta[k] += nodes[j].f_i; // Sumamos el poder de carga
                    }
                }
            }
        }
    };

    recalcular_cobertura();

    // =================================================================
    // FASE 2: Adición Greedy (Reparar demanda no satisfecha)
    // =================================================================
    bool factible = false;
    while (!factible) {
        factible = true;
        
        // Verificamos si algún nodo no cumple con su demanda requerida (D_i)
        for (uint j = 0; j < num_nodes; j++) {
            if (demanda_cubierta[j] < nodes[j].D_i) {
                factible = false;
                break;
            }
        }

        if (factible) break; // Si toda la demanda está cubierta, la solución es válida

        uint mejor_nodo = num_nodes;
        double mejor_ratio = -1.0;

        // Buscamos la mejor estación a instalar evaluando el costo/beneficio
        for (uint j = 0; j < num_nodes; j++) {
            if (!sol[j]) {
                uint beneficio_demanda = 0;
                
                // Calculamos cuánta demanda insatisfecha cubriría instalar una estación aquí
                for (uint k = 0; k < num_nodes; k++) {
                    if (distancias[j][k] <= nodes[j].S_i) {
                        if (demanda_cubierta[k] < nodes[k].D_i) {
                            uint falta = nodes[k].D_i - demanda_cubierta[k];
                            // El beneficio máximo está limitado por el poder de carga (f_i) de la estación
                            beneficio_demanda += std::min(falta, nodes[j].f_i);
                        }
                    }
                }

                if (beneficio_demanda > 0) {
                    // Heurística Greedy: Beneficio (demanda suplida) dividido por Costo de instalación (C_i)
                    double ratio = static_cast<double>(beneficio_demanda) / static_cast<double>(nodes[j].C_i);
                    if (ratio > mejor_ratio) {
                        mejor_ratio = ratio;
                        mejor_nodo = j;
                    }
                }
            }
        }

        // Evitar bucles infinitos si no existe un nodo capaz de mejorar la red
        if (mejor_nodo == num_nodes) break;

        // Aplicamos la instalación de la estación en el mejor nodo encontrado
        sol[mejor_nodo] = true;
        recalcular_cobertura();
    }

    // =================================================================
    // FASE 3: Poda Greedy (Eliminación de redundancias)
    // Apagamos estaciones que no afecten la factibilidad para bajar costos
    // =================================================================
    for (uint j = 0; j < num_nodes; j++) {
        // No intentamos podar los nodos marcados como 'is_far'
        if (sol[j] && !nodes[j].is_far) { 
            sol[j] = false; // Apagamos temporalmente
            recalcular_cobertura();
            
            bool sigue_factible = true;
            for (uint k = 0; k < num_nodes; k++) {
                if (demanda_cubierta[k] < nodes[k].D_i) {
                    sigue_factible = false;
                    break;
                }
            }

            // Si apagarla rompe la demanda, revertimos el cambio
            if (!sigue_factible) {
                sol[j] = true;
            }
            // Si la red sigue siendo factible, la estación se queda apagada ahorrando su costo C_i
        }
    }
}