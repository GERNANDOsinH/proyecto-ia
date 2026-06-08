#include "utils.hpp"
#include <vector>
#include <random>
#include <fstream>
#include <stdexcept>

/**
 * Método constructor, lee un archivo de texto para leer los parámetros de la ejecución.
 * @param src: Dirección del archivo de texto que debe leer.
 */
AE::AE(uint size_poblation, float beta, string src) {
    this->size_poblation = size_poblation;
    this->beta = beta;

    ifstream file(src);
    if (!file.is_open()) {
        throw runtime_error("Error: No se pudo abrir el archivo " + src);
    }
    if (!(file >> this->num_nodes >> this->R >> this->alpha >> this->CityArea)) {
        throw runtime_error("Error: Formato incorrecto en la primera línea del archivo");
    }
    this->nodes = vector<node>(num_nodes);
    this->matrix = vector<vector<bool>>(size_poblation, vector<bool>(num_nodes));
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
}

/**
 * Operador cruzamiento entre 2 individuos.
 * @param pos: Posición donde se aplicara el cruzamiento.
 * @param i  : Indice padre 1 que se cruzara.
 * @param j  : Indice padre 2 que se cruzara.
 * @param i_p: Indice hijo 1.
 * @param j_p: Indice hijo 2.
 */
void AE::cruce(int pos, int i, int j, int i_p, int j_p) {
    for (int k = 0;k < num_nodes;k++) {
        if (k < pos) {
            matrix[i_p][k] = matrix[i][k];
            matrix[j_p][k] = matrix[j][k];
        }
        else {
            matrix[i_p][k] = matrix[j][k];
            matrix[j_p][k] = matrix[i][k];
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
    bool rest2 = false; // La configuración actual es transitable.
    bool out = false;          // Retorna si se hizo un cambio.

    uint area_covered = 0;
    vector<option> options1; // Posibles elecciones, incluye todos los nodos.
    vector<option> options2; // Posibles elecciones, solo nodos lejanos.
    option x;
    for (int j = 0;j < num_nodes;j++) {
        if (!matrix[i][j]) {
            x = {j, ((float)nodes[j].s_i)/(nodes[j].c_i)};
            options1.push_back(x);
            if (nodes[j].is_far)
                options2.push_back(x);

        }
        area_covered += nodes[j].s_i;
        rest1 |= nodes[j].is_far;
    }
    if (!rest1) {
        sort(
            options2.begin(),
            options2.end(),
            [](const option& a, const option& b)
            { return a.coef < b.coef; });
        int j = options2[0].index;
        matrix[i][j] = true;
        area_covered += nodes[j].s_i;
        for (int k = 0;k < options1.size();k++) {
            if (options1[k].index == j) {
                options1.erase(options1.begin() + k);
                break;
            }
        }
        out = true;
    }
    sort(
        options1.begin(),
        options1.end(),
        [](const option& a, const option& b)
        { return a.coef < b.coef; });
    while (area_covered < CityArea || 0 < options1.size()) {
        int j = options1[0].index;
        area_covered += nodes[j].s_i;
        options1.erase(options1.begin());
        out = true;
    }
    
    for (int k = 0; k < num_nodes; k++) {
        uint current_capacity = 0;
        vector<int> candidates;

        for (int j = 0; j < num_nodes; j++) {
            if (dist[k][j] <= alpha) { // j está dentro del radio de cobertura alpha de k
                if (matrix[i][j]) {
                    current_capacity += nodes[j].f_i; // Sumar capacidad de nodos ya instalados
                } else {
                    candidates.push_back(j); // Guardar nodos no instalados como candidatos
                }
            }
        }

        sort(candidates.begin(), candidates.end(), [&](int a, int b) {
            return (float)nodes[a].f_i / nodes[a].c_i > (float)nodes[b].f_i / nodes[b].c_i;
        });

        int cand_idx = 0;
        // Mientras la demanda no esté cubierta, activar nuevos cargadores cercanos
        while (current_capacity < nodes[k].D_i && cand_idx < candidates.size()) {
            int j = candidates[cand_idx++];
            matrix[i][j] = true;
            current_capacity += nodes[j].f_i;
            out = true;
        }
    }

    bool connected = false;
    while (!connected) {
        vector<int> selected;
        for (int j = 0; j < num_nodes; j++) {
            if (matrix[i][j]) selected.push_back(j);
        }
        if (selected.empty()) break; // Seguridad

        vector<bool> visited(num_nodes, false);
        vector<int> component;
        vector<int> q; // Cola para BFS (Búsqueda en Anchura)
        
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
            // más cercano a cualquier nodo de nuestra componente principal y activarlo para "construir un puente".
            int best_k = -1;
            float min_d = 1e9; // Simula infinito
            
            for (int u : component) {
                for (int k = 0; k < num_nodes; k++) {
                    if (!matrix[i][k]) {
                        if (dist[u][k] < min_d) {
                            min_d = dist[u][k];
                            best_k = k;
                        }
                    }
                }
            }
            
            if (best_k != -1) {
                matrix[i][best_k] = true;
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
    auto instance = matrix[i];
    uint out = 0;
    for (int j = 0;j < num_nodes;j++)
        out += (instance[j])? nodes[j].c_i : 0;
    
    return out;
}

void AE::solve(uint max_iterations) {
    uint iteration = max_iterations;
    float torta;
    uint index;

    // Aseguramos de que tengamos un generador de puntos de cruce válido (evitando extremos 0 o num_nodes)
    uniform_int_distribution<int> dist_cruce(1, num_nodes - 1);

    while (0 < iteration--) {
        float better = -1.0;
        torta = 0.0;
        
        // =================================================================
        // 1. Evaluación y Normalización
        // =================================================================
        for (int i = 0; i < size_poblation; i++) {
            repair(i);
            float current_FE = FE(i);
            
            // Maximización: menor costo original = mayor probabilidad FEs[i]
            FEs[i] = 1.0 / current_FE; 
            
            if (better < FEs[i]) {
                better = FEs[i];
                index = i;
            }
            torta += FEs[i];
        }
        
        for (int i = 0; i < size_poblation; i++) {
            FEs[i] /= torta; // Normalización para crear la distribución de probabilidad
        }

        // Elitismo: Guardamos la mejor configuración para que no se pierda por mutaciones o cruces
        auto mejor = matrix[index];

        // =================================================================
        // 2. Selección de Padres (Método de la Ruleta)
        // =================================================================
        vector<int> parents(size_poblation);
        for (int i = 0; i < size_poblation; i++) {
            float r = dist_float(motor); // Se asume que dist_float genera en [0.0, 1.0)
            float accum = 0.0;
            for (int j = 0; j < size_poblation; j++) {
                accum += FEs[j];
                if (r <= accum) {
                    parents[i] = j;
                    break;
                }
            }
        }

        // =================================================================
        // 3. Cruzamiento
        // =================================================================
        // Redimensionamos la matriz para agregar los hijos al final sin pisar a los padres
        matrix.resize(size_poblation * 2, vector<bool>(num_nodes));
        
        int num_pairs = size_poblation / 2;
        for (int i = 0; i < num_pairs; i++) {
            int p1 = parents[2 * i];
            int p2 = parents[2 * i + 1];
            
            int c1 = size_poblation + (2 * i);
            int c2 = size_poblation + (2 * i + 1);
            
            int pos = dist_cruce(motor); 
            
            cruce(pos, p1, p2, c1, c2); // Usamos tu operador original
        }

        // Si la población es impar, copiamos directamente un padre aleatorio al último espacio
        if (size_poblation % 2 != 0) {
            matrix[(size_poblation * 2) - 1] = matrix[parents.back()];
        }

        // =================================================================
        // 4. Mutación (Solo aplicada a los hijos)
        // =================================================================
        for (int i = size_poblation; i < size_poblation * 2; i++) {
            for (int j = 0; j < num_nodes; j++) {
                if (dist_float(motor) < beta) {
                    matrix[i][j] = !matrix[i][j]; // Flipeo de bit
                }
            }
        }

        // =================================================================
        // 5. Reemplazo Generacional
        // =================================================================
        // Eliminamos la primera mitad (padres antiguos)
        matrix.erase(matrix.begin(), matrix.begin() + size_poblation);

        // Reintroducimos la mejor solución en la nueva generación (Elitismo puro)
        matrix[0] = mejor;
    }
}
/**
 * Constructor de penalty_AE
 */
penalty_AE::penalty_AE(uint size_poblation, float beta, uint M1, uint M2, uint M3, string src) : AE(size_poblation, beta, src) {
    this->M1 = M1;
    this->M2 = M2;
    this->M3 = M3;
}

uint penalty_AE::FE(int i) {
    uint base_cost = 0;
    // 1. Calcular el costo base de los cargadores activados
    for (int j = 0; j < num_nodes; j++) {
        if (matrix[i][j]) {
            base_cost += nodes[j].c_i;
        }
    }

    uint total_penalties = 0;

    // --- RESTRICCIÓN 1: Al menos un nodo lejano ---
    bool has_far = false;
    for (int j = 0; j < num_nodes; j++) {
        if (matrix[i][j] && nodes[j].is_far) {
            has_far = true;
            break;
        }
    }
    if (!has_far) {
        total_penalties += M1;
    }

    // --- RESTRICCIÓN 2: Transitabilidad (Grafo Conexo con distancia <= R) ---
    vector<int> selected;
    for (int j = 0; j < num_nodes; j++) {
        if (matrix[i][j]) selected.push_back(j);
    }

    if (!selected.empty()) {
        vector<bool> visited(num_nodes, false);
        int components = 0;
        
        for (int s : selected) {
            if (!visited[s]) {
                components++;
                // Búsqueda en anchura (BFS) para identificar la componente conexa
                vector<int> q = {s};
                visited[s] = true;
                while (!q.empty()) {
                    int curr = q.front();
                    q.erase(q.begin());
                    for (int j : selected) {
                        if (!visited[j] && dist[curr][j] <= R) {
                            visited[j] = true;
                            q.push_back(j);
                        }
                    }
                }
            }
        }
        // Si hay más de una componente, el recorrido no es transitable de forma continua
        if (components > 1) {
            total_penalties += M2 * (components - 1);
        }
    } else {
        total_penalties += M2 * num_nodes; // Penalización máxima si no hay nodos
    }

    // --- RESTRICCIÓN 3: Cobertura de Área y Demanda Satisfecha ---
    uint area_covered = 0;
    for (int j : selected) {
        area_covered += nodes[j].s_i;
    }
    if (area_covered < CityArea) {
        total_penalties += M3 * (CityArea - area_covered);
    }

    // Verificación de Demanda por cada nodo del sistema
    for (int k = 0; k < num_nodes; k++) {
        uint current_capacity = 0;
        for (int j : selected) {
            if (dist[k][j] <= alpha) { // j está dentro del radio de cobertura alpha de k
                current_capacity += nodes[j].f_i;
            }
        }
        // Si la capacidad instalada no cubre la demanda D_i, penalizamos el déficit
        if (current_capacity < nodes[k].D_i) {
            total_penalties += M3 * (nodes[k].D_i - current_capacity);
        }
    }

    return base_cost + total_penalties;
}
/**
 * Itera el algoritmo evolutivo, a diferencia del normal este tiene capacidades de control para aumentar la penalización.
 * @param max_iterations: Iteraciones maximas permitidas.
 */
void penalty_AE::solve(uint max_iterations) {
    uint iteration = max_iterations;
    float torta;
    uint index;
    uniform_int_distribution<int> dist_cruce(1, num_nodes - 1);

    while (0 < iteration--) {
        float better = -1.0;
        torta = 0.0;
        uint feasible_count = 0;

        // =================================================================
        // 1. Evaluación y Monitoreo de Factibilidad
        // =================================================================
        for (int i = 0; i < size_poblation; i++) {
            uint total_cost = FE(i); // Utiliza la función sobreescrita con penalizaciones
            
            // Calculamos el costo base para verificar si tiene penalizaciones activas
            uint base_cost = 0;
            for (int j = 0; j < num_nodes; j++) {
                if (matrix[i][j]) base_cost += nodes[j].c_i;
            }

            // Si el costo total es igual al costo base, el individuo es 100% factible
            if (total_cost == base_cost && base_cost > 0) {
                feasible_count++;
            }

            FEs[i] = 1.0 / total_cost; // Maximización
            if (better < FEs[i]) {
                better = FEs[i];
                index = i;
            }
            torta += FEs[i];
        }

        // =================================================================
        // 2. Mecanismo de Control de Penalizaciones Dinámicas
        // =================================================================
        float feasible_ratio = (float)feasible_count / size_poblation;
        
        // Si menos del 15% de la población es factible, aumentamos la presión de selección
        if (feasible_ratio < 0.15) {
            M1 = (uint)(M1 * 1.5) + 1;
            M2 = (uint)(M2 * 1.5) + 1;
            M3 = (uint)(M3 * 1.5) + 1;
        } 
        // Si más del 80% de la población ya es factible, relajamos sutilmente las restricciones
        else if (feasible_ratio > 0.80 && M1 > 5 && M2 > 5 && M3 > 5) {
            M1 = (uint)(M1 * 0.9);
            M2 = (uint)(M2 * 0.9);
            M3 = (uint)(M3 * 0.9);
        }

        // Normalización para la ruleta
        for (int i = 0; i < size_poblation; i++) {
            FEs[i] /= torta;
        }

        // Guardar el mejor de la iteración (Elitismo)
        auto mejor = matrix[index];

        // =================================================================
        // 3. Selección por Ruleta
        // =================================================================
        vector<int> parents(size_poblation);
        for (int i = 0; i < size_poblation; i++) {
            float r = dist_float(motor);
            float accum = 0.0;
            for (int j = 0; j < size_poblation; j++) {
                accum += FEs[j];
                if (r <= accum) {
                    parents[i] = j;
                    break;
                }
            }
        }

        // =================================================================
        // 4. Operador de Cruzamiento
        // =================================================================
        matrix.resize(size_poblation * 2, vector<bool>(num_nodes));
        int num_pairs = size_poblation / 2;
        for (int i = 0; i < num_pairs; i++) {
            int p1 = parents[2 * i];
            int p2 = parents[2 * i + 1];
            int c1 = size_poblation + (2 * i);
            int c2 = size_poblation + (2 * i + 1);
            
            int pos = dist_cruce(motor);
            cruce(pos, p1, p2, c1, c2);
        }
        if (size_poblation % 2 != 0) {
            matrix[(size_poblation * 2) - 1] = matrix[parents.back()];
        }

        // =================================================================
        // 5. Operador de Mutación
        // =================================================================
        for (int i = size_poblation; i < size_poblation * 2; i++) {
            for (int j = 0; j < num_nodes; j++) {
                if (dist_float(motor) < beta) {
                    matrix[i][j] = !matrix[i][j];
                }
            }
        }

        // =================================================================
        // 6. Reemplazo y Elitismo
        // =================================================================
        matrix.erase(matrix.begin(), matrix.begin() + size_poblation);
        matrix[0] = mejor;
    }

    // =================================================================
    // Reparación Final Obligatoria
    // =================================================================
    // Al terminar todas las iteraciones, aplicamos el método repair a toda la 
    // población para asegurar de que cualquier residuo infactible sea corregido.
    for (int i = 0; i < size_poblation; i++) {
        repair(i);
    }
}