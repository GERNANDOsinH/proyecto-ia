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
    this->beta = beta; // IMPORTANTE: Ahora beta debe ser pequeño (ej. 1.0 / num_nodes)

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
    for (uint64_t i = 0; i < num_nodes; i++) {
        if (!(file >> trash >> nodes[i].c_i >> nodes[i].D_i >> nodes[i].f_i >> nodes[i].s_i >> nodes[i].is_far))
            throw runtime_error("Error: Una línea tiene un formato inválido.");
    }
    for (uint64_t i = 0; i < num_nodes; i++) {
        for (uint64_t j = 0; j < num_nodes; j++) {
            if (!(file >> dist[i][j]))
                throw runtime_error("Error al leer una distancia.");
        }
    }
    file.close();

    uniform_int_distribution<int> dist_bin(0, 1);
    
    for (uint64_t i = 0; i < this->size_poblation; i++) {
        for (uint64_t j = 0; j < this->num_nodes; j++) {
            this->matrix[i][j] = static_cast<uint8_t>(dist_bin(this->motor));
        }
        // Ya no llamamos a repair() al iniciar, dejamos que el algoritmo aprenda
    }
}
AE::~AE() {}
results AE::print() {
    results out = {1000000000ULL, 0};
    uint64_t current_FE;
    for (int i = 0;i < size_poblation;i++) {
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
    uniform_int_distribution<uint64_t> dist_point(0, num_nodes - 1);
    uint64_t p1 = dist_point(motor);
    uint64_t p2 = dist_point(motor);

    if (p1 > p2) swap(p1, p2);

    for (uint64_t k = 0; k < num_nodes; ++k) {
        if (k >= p1 && k <= p2) {
            next_gen[i_p][k] = matrix[j][k];
            next_gen[j_p][k] = matrix[i][k];
        } else {
            next_gen[i_p][k] = matrix[i][k];
            next_gen[j_p][k] = matrix[j][k];
        }
    }
}

/**
 * Función que evalúa la calidad de una solución mediante Costo y Penalizaciones.
 */
uint64_t AE::FE(int i) {
    uint64_t total_cost = 0;
    uint64_t area_covered = 0;
    std::vector<int> active_nodes;

    // 1. Calcular Costo Base y Área Cubierta
    for (uint64_t j = 0; j < num_nodes; j++) {
        if (matrix[i][j] == 1) {
            total_cost += nodes[j].c_i;
            area_covered += nodes[j].s_i;
            active_nodes.push_back(j);
        }
    }

    uint64_t penalty = 0;

    // 2. Penalización por Área Insuficiente
    if (CityArea > area_covered) {
        penalty += (CityArea - area_covered) * 5000ULL; // Ajusta este multiplicador
    }

    // 3. Penalización por Demanda no cubierta
    uint64_t demand_violation = 0;
    for (uint64_t k = 0; k < num_nodes; k++) {
        uint64_t current_capacity = 0;
        for (int j : active_nodes) {
            if (dist[k][j] <= alpha) {
                current_capacity += nodes[j].f_i;
            }
        }
        if (current_capacity < nodes[k].D_i) {
            demand_violation += (nodes[k].D_i - current_capacity);
        }
    }
    penalty += demand_violation * 10000ULL; // Ajusta este multiplicador

    // 4. Penalización por Desconexión (BFS rápido)
    if (!active_nodes.empty()) {
        vector<bool> visited(num_nodes, false);
        queue<int> q;
        
        q.push(active_nodes[0]);
        visited[active_nodes[0]] = true;
        int connected_count = 0;

        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            connected_count++;

            for (int j : active_nodes) {
                if (!visited[j] && dist[curr][j] <= R) {
                    visited[j] = true;
                    q.push(j);
                }
            }
        }

        // Si no todos los nodos activos están en la misma componente conexa
        if (connected_count < active_nodes.size()) {
            uint64_t disconnected_nodes = active_nodes.size() - connected_count;
            penalty += disconnected_nodes * 50000ULL; // Castigo muy severo
        }
    } else {
        // Red vacía (castigo masivo para evitar que converja a todo ceros)
        penalty += 10000000ULL;
    }

    return total_cost + penalty;
}

void AE::iteration() {
    // 1. Primero calcular el total y el mejor individuo.
    double total = 0.0;
    double better_FE = -1.0;
    uint64_t better_index = 0;
    for (uint64_t i = 0; i < size_poblation; i++) {
        // Al usar penalizaciones, FE(i) será muy alto en individuos malos. 
        // Invertirlo sigue funcionando para la ruleta.
        FEs[i] = 1.0 / ((double)FE(i) + 1e-6);
        total += FEs[i];
        if (better_FE < FEs[i]) {
            better_FE = FEs[i];
            better_index = i;
        }
    }
    
    // 2. Normalización
    for (uint64_t i = 0; i < size_poblation; i++)
        FEs[i] /= total;
    
    // 3. Movemos el mejor individuo (Elitismo)
    if (better_index != 0) { 
        swap(matrix[0], matrix[better_index]);
        swap(FEs[0], FEs[better_index]);
    }

    vector<double> accum_prob(size_poblation);
    accum_prob[0] = FEs[0];
    for (uint64_t i = 1; i < size_poblation; i++)
        accum_prob[i] = accum_prob[i - 1] + FEs[i];
    accum_prob[size_poblation - 1] = 1.0;

    vector<int> parents(size_poblation);
    for (uint64_t i = 0; i < size_poblation; i++) {
        double ball = dist_double(motor);
        auto it = lower_bound(accum_prob.begin(), accum_prob.end(), ball);
        int chosen_parent = distance(accum_prob.begin(), it);
        if (chosen_parent >= size_poblation) {
            chosen_parent = size_poblation - 1;
        }
        parents[i] = chosen_parent;
    }

    vector<vector<uint8_t>> next_gen(size_poblation, vector<uint8_t>(num_nodes));
    next_gen[0] = matrix[0]; // Preservar el mejor individuo
    
    uint64_t num_pairs = (size_poblation - 1) / 2; 
    for (uint64_t i = 0; i < num_pairs; i++) {
        int p1 = parents[2 * i];
        int p2 = parents[2 * i + 1];
        int c1 = 2 * i + 1;
        int c2 = 2 * i + 2;
        cruce(p1, p2, c1, c2, next_gen);
    }
    if (size_poblation % 2 == 0)
        next_gen.back() = matrix[parents.back()];

    // 4. Mutación Gen por Gen
    for (uint64_t i = 1; i < size_poblation; i++) {    
        for (uint64_t j = 0; j < num_nodes; j++) {
            if (dist_double(motor) < beta) {
                next_gen[i][j] = (next_gen[i][j] == 1) ? 0 : 1;
            }
        }
        matrix[i] = move(next_gen[i]);
        // Se ha eliminado completamente la llamada a repair(i)
    }
}

void AE::solve(uint64_t max_iterations) {
    uint64_t iter = max_iterations;
    while (iter--)
        iteration();
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