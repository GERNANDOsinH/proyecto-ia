#include "algorithm.hpp"
#include <fstream>
#include <stdexcept>
#include <random>

using namespace std;
/**
 * Método constructor.
 * @param size_poblation: Tamaño de población que se usara.
 * @param beta: Probabilidad de mutaciones.
 * @param src: Dirección donde se encuntra un archivo de texto con los datos del problema.
 */
AE::AE(uint size_poblation, uint P1, uint P2, uint P3, double beta, string src) {
    this->penalty_1 = P1;
    this->penalty_2 = P2;
    this->penalty_3 = P3;
    this->size_poblation = size_poblation;
    this->beta = beta;
    this->max_cost = 0;

    random_device rd;
    rng.seed(rd());

    // === Lectura archivo de texto === //
    ifstream file(src);
    if (!file.is_open())
        throw runtime_error("Error: No se pudo abrir el archivo " + src);
    
    // Agregamos una variable extra para absorber el '100' del final de la primera línea
    double extra_val; 
    if (!(file >> this->num_nodes >> this->R >> this->alpha >> extra_val))
        throw std::runtime_error("Error: Formato incorrecto en la primera linea del archivo.");
    
    nodes = vector<node>(num_nodes);
    distancias = vector<vector<double>>(num_nodes, vector<double>(num_nodes));

    for (uint i = 0;i < num_nodes;i++) {
        uint id_nodo; // Usamos una variable temporal para leer el ID, sin tocar la 'i'
        if (!(file >> id_nodo >> nodes[i].C_i >> nodes[i].D_i >> nodes[i].f_i >> nodes[i].S_i >> nodes[i].is_far))
            throw runtime_error("Error: Una línea de nodo es inválida");
    }

    for (uint i = 0;i < num_nodes;i++) {
        for (uint j = 0;j < num_nodes;j++) {
            if (!(file >> distancias[i][j]))
                throw runtime_error("Error al leer una distancia");
        }
    }

    file.close();
    // === Fin lectura archivo de text === //
    
    // === Inicio generación inicial === //
    poblacion.resize(size_poblation, vector<bool>(num_nodes, false));

    uniform_real_distribution<double> density_dist(0.05, 0.25); 
    uniform_real_distribution<double> coin_flip(0.0, 1.0);

    for (uint i = 0;i < size_poblation;i++) {
        double current_density = density_dist(rng);
        
        for (uint j = 0;j < num_nodes;j++)
            poblacion[i][j] = (coin_flip(rng) < current_density)? true : false;
    }

    for (uint j = 0;j < num_nodes;j++)
        max_cost += nodes[j].C_i;
}