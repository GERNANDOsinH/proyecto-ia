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
AE::AE(uint size_poblation, double beta, string src) {
    this->size_poblation = size_poblation;
    this->beta = beta;

    // === Lectura archivo de texto === //
    ifstream file(src);
    if (!file.is_open())
        throw runtime_error("Error: No se pudo abrir el archivo " + src);
    if (!(file >> this->num_nodes >> this->R >> this->alpha))
        throw std::runtime_error("Error: Formato incorrecto en la primera linea del archivo.");
    
    nodes = vector<node>(num_nodes);
    distancias = vector<vector<double>>(num_nodes, vector<double>(num_nodes));

    for (uint i = 0;i < num_nodes;i++)
        if (!(file >> i >> nodes[i].C_i >> nodes[i].D_i >> nodes[i].f_i >> nodes[i].S_i >> nodes[i].is_far))
            throw runtime_error("Error: Una línea es inválida");

    for (uint i = 0;i < num_nodes;i++)
        for (uint j = 0;j < num_nodes;j++)
            if (!(file >> distancias[i][j]))
                throw runtime_error("Error al leer una distancia");

    file.close();
    // === Fin lectura archivo de text === //
    
    // === Inicio generación inicial === //
    poblacion.resize(size_poblation, vector<bool>(num_nodes, false));

    random_device rd;
    mt19937 gen(rd());

    uniform_real_distribution<double> density_dist(0.05, 0.25); 
    uniform_real_distribution<double> coin_flip(0.0, 1.0);

    for (uint i = 0;i < size_poblation;i++) {
        double current_density = density_dist(gen);
        
        for (uint j = 0;j < num_nodes;j++)
            poblacion[i][j] = (coin_flip(gen) < current_density)? true : false;
    }
}