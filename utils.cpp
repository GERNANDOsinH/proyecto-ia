#include "utils.hpp"
#include <fstream>
#include <stdexcept>

using namespace std;

/**
 * Método constructor, lee un archivo de texto para leer los parámetros de la ejecución.
 * @param src: Dirección del archivo de texto que debe leer.
 */
AE::AE(uint size_poblation, float beta, string src) {
    this->size_poblation = size_poblation;
    this->beta = beta;

    random_device rd;
    this->motor.seed(rd());
    this->dist_float = uniform_real_distribution<float>(0.0f, 1.0f);

    ifstream file(src);
    if (!file.is_open()) {
        throw runtime_error("Error: No se pudo abrir el archivo " + src);
    }
    if (!(file >> this->num_nodes >> this->R >> this->alpha >> this->CityArea)) {
        throw runtime_error("Error: Formato incorrecto en la primera línea del archivo");
    }
    this->nodes = vector<node>(num_nodes);
    this->FEs = vector<float>(size_poblation);
    this->matrix = vector<vector<uint8_t>>(size_poblation, vector<uint8_t>(num_nodes));
    this->dist = vector<vector<float>>(num_nodes, vector<float>(num_nodes));
    
    int trash;
    for (uint i = 0;i < num_nodes;i++) {
        if (!(file >> trash >> nodes[i].c_i >> nodes[i].D_i >> nodes[i].f_i >> nodes[i].s_i >> nodes[i].is_far))
            throw runtime_error("Error: Una línea tiene un formato inválido.");
    }
    for (uint i = 0;i < num_nodes;i++) {
        for (uint j = 0;j < num_nodes;j++) {
            if (!(file >> dist[i][j]))
                throw runtime_error("Error al leer una distancia.");
        }
    }
    file.close();

    uniform_int_distribution<int> dist_bin(0, 1);
    
    for (uint i = 0; i < this->size_poblation; i++) {
        for (uint j = 0; j < this->num_nodes; j++) {
            // Asigna aleatoriamente 0 (apagado) o 1 (encendido)
            this->matrix[i][j] = static_cast<uint8_t>(dist_bin(this->motor));
        }
    }
}
AE::~AE() {}
/**
 * Operador cruzamiento entre 2 individuos.
 * @param pos: Posición donde se aplicara el cruzamiento.
 * @param i  : Indice padre 1 que se cruzara.
 * @param j  : Indice padre 2 que se cruzara.
 * @param i_p: Indice hijo 1.
 * @param j_p: Indice hijo 2.
 */
void AE::cruce(int pos, int i, int j, int i_p, int j_p, vector<vector<uint8_t>>& next_gen) {
    for (uint k = 0; k < num_nodes; k++) {
        if (k < pos) {
            next_gen[i_p][k] = matrix[i][k];
            next_gen[j_p][k] = matrix[j][k];
        } else {
            next_gen[i_p][k] = matrix[j][k];
            next_gen[j_p][k] = matrix[i][k];
        }
    }
}

/** 
* Función que comprueba si la solución actual es factible y la repara en caso contrario.
* @param i: Individuo de la población que se quiere reparar.
* @return True si se reparo la instancia porque es infactible.
*/
bool AE::repair(int i) {
    bool rest1 = false; // Se asigno una estación de carga a un nodo lejano.
    bool out = false;   // Retorna si se hizo un cambio.

    uint area_covered = 0;
    std::vector<option> options1; // Posibles elecciones, incluye todos los nodos.
    std::vector<option> options2; // Posibles elecciones, solo nodos lejanos.
    option x;
    
    for (uint j = 0; j < num_nodes; j++) {
        if (matrix[i][j] == 0) { // Adaptado a uint8_t
            x = {(int)j, ((float)nodes[j].s_i)/(nodes[j].c_i)};
            options1.push_back(x);
            if (nodes[j].is_far)
                options2.push_back(x);
        }
        // Nota: en tu código original el área cubierta se sumaba para TODOS los nodos,
        // sin importar si estaban seleccionados (matrix[i][j] == 1) o no.
        area_covered += nodes[j].s_i;
        rest1 |= nodes[j].is_far;
    }
    
    if (!rest1) {
        std::sort(
            options2.begin(),
            options2.end(),
            [](const option& a, const option& b)
            { return a.coef < b.coef; });
            
        int j = options2[0].index;
        matrix[i][j] = 1; // Cambiado de true a 1
        area_covered += nodes[j].s_i;
        
        for (int k = 0; k < options1.size(); k++) {
            if (options1[k].index == j) {
                options1.erase(options1.begin() + k);
                break;
            }
        }
        out = true;
    }
    
    std::sort(
        options1.begin(),
        options1.end(),
        [](const option& a, const option& b)
        { return a.coef < b.coef; });
        
    while (area_covered < CityArea || 0 < options1.size()) {
        int j = options1[0].index;
        
        // OJO: En tu código original, este bucle no activa el cargador en la matriz.
        // Solo aumenta el área cubierta y lo borra de las opciones. 
        // Si tu intención era activarlo, te falta descomentar o agregar la línea de abajo:
        // matrix[i][j] = 1; 

        area_covered += nodes[j].s_i;
        options1.erase(options1.begin());
        out = true;
    }
    
    for (uint k = 0; k < num_nodes; k++) {
        uint current_capacity = 0;
        std::vector<int> candidates;

        for (uint j = 0; j < num_nodes; j++) {
            if (dist[k][j] <= alpha) { // j está dentro del radio de cobertura alpha de k
                if (matrix[i][j] == 1) { // Adaptado a uint8_t
                    current_capacity += nodes[j].f_i; // Sumar capacidad de nodos ya instalados
                } else {
                    candidates.push_back(j); // Guardar nodos no instalados como candidatos
                }
            }
        }

        std::sort(candidates.begin(), candidates.end(), [&](int a, int b) {
            return (float)nodes[a].f_i / nodes[a].c_i > (float)nodes[b].f_i / nodes[b].c_i;
        });

        int cand_idx = 0;
        // Mientras la demanda no esté cubierta, activar nuevos cargadores cercanos
        while (current_capacity < nodes[k].D_i && cand_idx < candidates.size()) {
            int j = candidates[cand_idx++];
            matrix[i][j] = 1; // Cambiado de true a 1
            current_capacity += nodes[j].f_i;
            out = true;
        }
    }

    bool connected = false;
    while (!connected) {
        std::vector<int> selected;
        for (uint j = 0; j < num_nodes; j++) {
            if (matrix[i][j] == 1) selected.push_back(j); // Adaptado a uint8_t
        }
        if (selected.empty()) break; // Seguridad

        std::vector<bool> visited(num_nodes, false);
        std::vector<int> component;
        std::vector<int> q; // Cola para BFS (Búsqueda en Anchura)
        
        q.push_back(selected[0]);
        visited[selected[0]] = true;

        // Construir la componente conexa principal
        while (!q.empty()) {
            int curr = q.front();
            q.erase(q.begin());
            component.push_back(curr);

            for (int j : selected) {
                // Si la distancia al siguiente nodo seleccionado es <= R, es transitable
                if (!visited[j] && dist[curr][j] <= R) {
                    visited[j] = true;
                    q.push_back(j);
                }
            }
        }

        // Verificar si todos los nodos seleccionados pertenecen a la misma ruta
        if (component.size() == selected.size()) {
            connected = true; 
        } else {
            // Reparación Codiciosa (Greedy): Si no es conexo, buscar el nodo NO seleccionado
            // más cercano a cualquier nodo de nuestra componente principal y activarlo.
            int best_k = -1;
            float min_d = 1e9; // Simula infinito
            
            for (int u : component) {
                for (uint k = 0; k < num_nodes; k++) {
                    if (matrix[i][k] == 0) { // Adaptado a uint8_t
                        if (dist[u][k] < min_d) {
                            min_d = dist[u][k];
                            best_k = k;
                        }
                    }
                }
            }
            
            if (best_k != -1) {
                matrix[i][best_k] = 1; // Cambiado de true a 1
                out = true;
            } else {
                break; // Fallback: Evitar bucle si el mapa físico no permite conexión
            }
        }
    }

    return out;
}

/**
 * Función que mide la calidad de una solución particular.
 * @param i: Individuo de la población que se quiere evaluar.
 * @return Costo asociado a una instancia en particular.
 */
uint AE::FE(int i) {
    uint out = 0;
    for (uint j = 0;j < num_nodes;j++)
        out += (matrix[i][j])? nodes[j].c_i : 0;
    
    return out;
}

void AE::solve(uint max_iterations) {
    uint iteration = max_iterations;
    float torta;
    uint index = 0;

    uniform_int_distribution<int> dist_cruce(1, num_nodes - 1);

    while (0 < iteration--) {
        float better = -1.0;
        torta = 0.0;
        
        for (uint i = 0; i < size_poblation; i++) {
            repair(i);
            float current_FE = FE(i);
            
            FEs[i] = 1.0 / current_FE; 
            
            if (better < FEs[i]) {
                better = FEs[i];
                index = i;
            }
            torta += FEs[i];
        }
        
        for (uint i = 0; i < size_poblation; i++) {
            FEs[i] /= torta; 
        }

        auto mejor = matrix[index];

        vector<int> parents(size_poblation);
        for (uint i = 0; i < size_poblation; i++) {
            float r = dist_float(motor); 
            float accum = 0.0;
            for (uint j = 0; j < size_poblation; j++) {
                accum += FEs[j];
                if (r <= accum) {
                    parents[i] = j;
                    break;
                }
            }
        }

        vector<vector<uint8_t>> next_gen(size_poblation, vector<uint8_t>(num_nodes));
        
        int num_pairs = size_poblation / 2;
        for (int i = 0; i < num_pairs; i++) {
            int p1 = parents[2 * i];
            int p2 = parents[2 * i + 1];
            
            int c1 = 2 * i;
            int c2 = 2 * i + 1;
            
            int pos = dist_cruce(motor); 
            cruce(pos, p1, p2, c1, c2, next_gen);
        }

        if (size_poblation % 2 != 0) {
            next_gen.back() = matrix[parents.back()];
        }

        for (uint i = 0; i < size_poblation; i++) {
            for (uint j = 0; j < num_nodes; j++) {
                if (dist_float(motor) < beta) {
                    next_gen[i][j] = next_gen[i][j] == 1 ? 0 : 1; // Flip
                }
            }
        }

        matrix = std::move(next_gen);
        matrix[0] = mejor;
    }
}
void AE::save(std::string src) {
    std::ofstream file(src);
    
    if (!file.is_open()) {
        throw std::runtime_error("Error: No se pudo abrir o crear el archivo para guardar " + src);
    }

    // Iterar sobre cada individuo de la población
    for (uint i = 0; i < size_poblation; i++) {
        // Iterar sobre cada gen (nodo) del individuo
        for (uint j = 0; j < num_nodes; j++) {
            // Se castea a int para que se escriba como texto '0' o '1' y no como un caracter ASCII
            file << static_cast<int>(matrix[i][j]);
            
            // Agregar un espacio entre los valores, excepto al final de la línea
            if (j < num_nodes - 1) {
                file << " ";
            }
        }
        // Salto de línea para el siguiente individuo
        file << "\n";
    }

    file.close();
}
/**
 * Constructor de penalty_AE
 */
penalty_AE::penalty_AE(uint size_poblation, float beta, uint M1, uint M2, uint M3, string src) : AE(size_poblation, beta, src) {
    this->M1 = M1;
    this->M2 = M2;
    this->M3 = M3;
}
penalty_AE::~penalty_AE() {}
uint penalty_AE::FE(int i, uint& out_base_cost) {
    out_base_cost = 0;
    for (uint j = 0; j < num_nodes; j++) {
        if (matrix[i][j]) {
            out_base_cost += nodes[j].c_i;
        }
    }
    
    uint total_penalties = 0;
    return out_base_cost + total_penalties;
}
uint penalty_AE::FE(int i) {
    uint dummy;
    return FE(i, dummy);
}
/**
 * Itera el algoritmo evolutivo, a diferencia del normal este tiene capacidades de control para aumentar la penalización.
 * @param max_iterations: Iteraciones maximas permitidas.
 */
void penalty_AE::solve(uint max_iterations) {
    uint iteration = max_iterations;
    float torta;
    uint index = 0;
    uniform_int_distribution<int> dist_cruce(1, num_nodes - 1);

    while (0 < iteration--) {
        float better = -1.0;
        torta = 0.0;
        uint feasible_count = 0;

        for (uint i = 0; i < size_poblation; i++) {
            uint base_cost = 0;
            // Evaluamos extrayendo el costo base simultáneamente
            uint total_cost = FE(i, base_cost); 

            if (total_cost == base_cost && base_cost > 0) {
                feasible_count++;
            }

            FEs[i] = 1.0 / total_cost;
            if (better < FEs[i]) {
                better = FEs[i];
                index = i;
            }
            torta += FEs[i];
        }

        float feasible_ratio = (float)feasible_count / size_poblation;
        if (feasible_ratio < 0.15) {
            M1 = (uint)(M1 * 1.5) + 1;
            M2 = (uint)(M2 * 1.5) + 1;
            M3 = (uint)(M3 * 1.5) + 1;
        } else if (feasible_ratio > 0.80 && M1 > 5 && M2 > 5 && M3 > 5) {
            M1 = (uint)(M1 * 0.9);
            M2 = (uint)(M2 * 0.9);
            M3 = (uint)(M3 * 0.9);
        }

        for (uint i = 0; i < size_poblation; i++) {
            FEs[i] /= torta;
        }

        auto mejor = matrix[index];

        vector<int> parents(size_poblation);
        for (uint i = 0; i < size_poblation; i++) {
            float r = dist_float(motor);
            float accum = 0.0;
            for (uint j = 0; j < size_poblation; j++) {
                accum += FEs[j];
                if (r <= accum) {
                    parents[i] = j;
                    break;
                }
            }
        }

        // --- CORRECCIÓN: Doble búfer en Penalty AE ---
        vector<vector<uint8_t>> next_gen(size_poblation, vector<uint8_t>(num_nodes));
        int num_pairs = size_poblation / 2;
        
        for (int i = 0; i < num_pairs; i++) {
            int p1 = parents[2 * i];
            int p2 = parents[2 * i + 1];
            int c1 = 2 * i;
            int c2 = 2 * i + 1;
            
            int pos = dist_cruce(motor);
            cruce(pos, p1, p2, c1, c2, next_gen);
        }
        
        if (size_poblation % 2 != 0) {
            next_gen.back() = matrix[parents.back()];
        }

        for (uint i = 0; i < size_poblation; i++) {
            for (uint j = 0; j < num_nodes; j++) {
                if (dist_float(motor) < beta) {
                    next_gen[i][j] = next_gen[i][j] == 1 ? 0 : 1;
                }
            }
        }

        matrix = std::move(next_gen);
        matrix[0] = mejor;
    }

    for (uint i = 0; i < size_poblation; i++) {
        repair(i);
    }
}