#include "utils.hpp"
#include <fstream>
#include <stdexcept>

using namespace std;

/**
 * Método constructor, lee un archivo de texto para leer los parámetros de la ejecución.
 * @param src: Dirección del archivo de texto que debe leer.
 */
AE::AE(uint64_t size_poblation, double beta, string src) {
    this->size_poblation = size_poblation;
    this->beta = beta;

    random_device rd;
    this->motor.seed(rd());
    this->dist_double = uniform_real_distribution<double>(0.0f, 1.0f);

    ifstream file(src);
    if (!file.is_open()) {
        throw runtime_error("Error: No se pudo abrir el archivo " + src);
    }
    if (!(file >> this->num_nodes >> this->R >> this->alpha >> this->CityArea)) {
        throw runtime_error("Error: Formato incorrecto en la primera línea del archivo");
    }
    this->nodes = vector<node>(num_nodes);
    this->FEs = vector<double>(size_poblation);
    this->matrix = vector<vector<uint8_t>>(size_poblation, vector<uint8_t>(num_nodes));
    this->dist = vector<vector<double>>(num_nodes, vector<double>(num_nodes));
    
    int trash;
    for (uint64_t i = 0;i < num_nodes;i++) {
        if (!(file >> trash >> nodes[i].c_i >> nodes[i].D_i >> nodes[i].f_i >> nodes[i].s_i >> nodes[i].is_far))
            throw runtime_error("Error: Una línea tiene un formato inválido.");
    }
    for (uint64_t i = 0;i < num_nodes;i++) {
        for (uint64_t j = 0;j < num_nodes;j++) {
            if (!(file >> dist[i][j]))
                throw runtime_error("Error al leer una distancia.");
        }
    }
    file.close();

    uniform_int_distribution<int> dist_bin(0, 1);
    
    for (uint64_t i = 0; i < this->size_poblation; i++) {
        for (uint64_t j = 0; j < this->num_nodes; j++) {
            // Asigna aleatoriamente 0 (apagado) o 1 (encendido)
            this->matrix[i][j] = static_cast<uint8_t>(dist_bin(this->motor));
        }
        repair(i);
    }
}
AE::~AE() {}
results AE::print() {
    results out = {1e9, 0};
    uint64_t current_FE;
    for (int i = 0;i < num_nodes;i++) {
        current_FE = FE(i);
        out.better_FE = (current_FE < out.better_FE)? current_FE : out.better_FE;
        out.average_FE += current_FE;
    }
    out.average_FE /= num_nodes;
    return out;
}
/**
 * Operador cruzamiento por punto entre 2 individuos.
 * @param i  : Indice padre 1 que se cruzara.
 * @param j  : Indice padre 2 que se cruzara.
 * @param i_p: Indice hijo 1.
 * @param j_p: Indice hijo 2.
 */
void AE::cruce(int i, int j, int i_p, int j_p, vector<vector<uint8_t>>& next_gen) {
    for (uint64_t k = 0; k < num_nodes; ++k) {
        if (dist_double(motor) < 0.5) {
            next_gen[i_p][k] = matrix[i][k];
            next_gen[j_p][k] = matrix[j][k];
        } 
        else {
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

    uint64_t area_covered = 0;
    std::vector<option> options1; // Posibles elecciones, incluye todos los nodos.
    std::vector<option> options2; // Posibles elecciones, solo nodos lejanos.
    option x;
    
    for (uint64_t j = 0; j < num_nodes; j++) {
        if (matrix[i][j] == 0) { // Adaptado a uint8_t
            x = {(int)j, ((double)nodes[j].s_i)/(nodes[j].c_i)};
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
        
    while (area_covered < CityArea && 0 < options1.size()) {
        int j = options1[0].index;
        
        matrix[i][j] = 1; 

        area_covered += nodes[j].s_i;
        options1.erase(options1.begin());
        out = true;
    }
    
    for (uint64_t k = 0; k < num_nodes; k++) {
        uint64_t current_capacity = 0;
        std::vector<int> candidates;

        for (uint64_t j = 0; j < num_nodes; j++) {
            if (dist[k][j] <= alpha) { // j está dentro del radio de cobertura alpha de k
                if (matrix[i][j] == 1) { // Adaptado a uint8_t
                    current_capacity += nodes[j].f_i; // Sumar capacidad de nodos ya instalados
                } else {
                    candidates.push_back(j); // Guardar nodos no instalados como candidatos
                }
            }
        }

        std::sort(candidates.begin(), candidates.end(), [&](int a, int b) {
            return (double)nodes[a].f_i / nodes[a].c_i > (double)nodes[b].f_i / nodes[b].c_i;
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
        for (uint64_t j = 0; j < num_nodes; j++) {
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
            double min_d = 1e9; // Simula infinito
            
            for (int u : component) {
                for (uint64_t k = 0; k < num_nodes; k++) {
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
uint64_t AE::FE(int i) {
    uint64_t out = 0;
    for (uint64_t j = 0;j < num_nodes;j++)
        out += (matrix[i][j])? nodes[j].c_i : 0;
    
    return out;
}

void AE::solve(uint64_t max_iterations) {
    uint64_t iteration = max_iterations;
    double torta;
    uint64_t index = 0;

    uniform_int_distribution<int> dist_cruce(1, num_nodes - 1);

    while (0 < iteration--) {
        double better = -1.0;
        torta = 0.0;
        
        for (uint64_t i = 1; i < size_poblation; i++) {
            repair(i);
            double current_FE = FE(i);
            
            FEs[i] = 1.0 / (current_FE + 0.000001f);
            
            if (better < FEs[i]) {
                better = FEs[i];
                index = i;
            }
            torta += FEs[i];
        }
        
        for (uint64_t i = 0; i < size_poblation; i++) {
            FEs[i] /= torta; 
        }

        auto mejor = matrix[index];

        vector<int> parents(size_poblation);
        for (uint64_t i = 0; i < size_poblation; i++) {
            double r = dist_double(motor); 
            double accum = 0.0;
            for (uint64_t j = 0; j < size_poblation; j++) {
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

            cruce(p1, p2, c1, c2, next_gen);
        }

        if (size_poblation % 2 != 0) {
            next_gen.back() = matrix[parents.back()];
        }

        for (uint64_t i = 0; i < size_poblation; i++) {
            for (uint64_t j = 0; j < num_nodes; j++) {
                if (dist_double(motor) < beta) {
                    next_gen[i][j] = next_gen[i][j] == 1 ? 0 : 1; // Flip
                }
            }
        }

        matrix = std::move(next_gen);
        matrix[0] = mejor;
    }
}

/**
 * Operador cruzamiento uniforme entre 2 individuos.
 * @param i  : Indice padre 1 que se cruzara.
 * @param j  : Indice padre 2 que se cruzara.
 * @param i_p: Indice hijo 1.
 * @param j_p: Indice hijo 2.
 */
void uniform_AE::cruce(int i, int j, int i_p, int j_p, vector<vector<uint8_t>>& next_gen) {
    for (uint64_t k = 0; k < num_nodes; ++k) {
        if (dist_double(motor) < 0.5) {
            next_gen[i_p][k] = matrix[i][k];
            next_gen[j_p][k] = matrix[j][k];
        } 
        else {
            next_gen[i_p][k] = matrix[j][k];
            next_gen[j_p][k] = matrix[i][k];
        }
    }
}