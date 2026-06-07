#include <iostream>
#include <string>
#include <vector>

#define uint unsigned int

using namespace std;

struct node {
    bool is_far; // Declara si el nodo es lejano o no.
    uint f_i;    // Capacidad de carga del nodo(si se coloca un cargador).
    uint c_i;    // Costo asociado a colocar un punto de carga en el nodo.
    uint D_i;    // Demanda del nodo i.
    uint s_i;     // Rango del nodo i.
};

class AE {
    private:
        // ==== Atributos algoritmo ==== // 
        float alpha;                 // Probabilidad de una mutación.
        uint max_iterations;         // Maximas iteraciones permitidas.

        // ==== Atributos problema ==== //
        uint CityArea;               // Área de la ciudad que se desea cubrir.
        uint R;                      // Autonomia promedio de los vehiculos.
        uint num_nodes;              // Número de nodos disponibles, incluyendo nodos lejanos.
        uint size_poblation;         // Tamaño de la población.

        vector<node> nodes;          // Nodos no lejanos.
        vector<vector<bool>> matrix; // Matriz población.
        vector<uint> FEs;            // Función de evaluación de cada instancia en la población.
        vector<vector<uint>> dist;   // Distancia entre todos los nodos.
    public:
        /**
         * Método constructor, lee un archivo de texto para leer los parámetros de la ejecución.
         * @param src: Dirección del archivo de texto que debe leer.
         */
        AE(string src) {
            // ...
        }
        ~AE();
        /** 
        * Función que comprueba si la solución actual es factible y la repara en caso contrario.
        * @param i: Individuo de la población que se quiere reparar.
        * @return True si se reparo la instancia porque es infactible.
        */
        bool repair(int i) {
            // ...
        }
        /**
         * Función que mide la calidad de una solución particular.
         * @param i: Individuo de la población que se quiere evaluar.
         * @return Costo asociado a una instancia en particular.
         */
        uint FE(int i) {
            auto instance = matrix[i];
            int out = 0;
            for (int j = 0;j < num_nodes;j++)
                out += (instance[j])? nodes[j].c_i : 0;
            
            return out;
            
        }
        // Función que ejecuta el Algoritmo Evolutivo.
        void solve() {
            uint i = max_iterations;
            while (0 < i--) {

            }
        }
};