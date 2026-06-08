#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

#define uint unsigned int

using namespace std;

struct node {
    bool is_far; // Declara si el nodo es lejano o no.
    uint f_i;    // Capacidad de carga del nodo(si se coloca un cargador).
    uint c_i;    // Costo asociado a colocar un punto de carga en el nodo.
    uint D_i;    // Demanda del nodo i.
    uint s_i;    // Rango del nodo i.
};

class AE {
    private:
        // ==== Atributos algoritmo ==== // 
        float alpha;                 // Probabilidad de una mutación.
        uint max_iterations;         // Maximas iteraciones permitidas.
        random_device seed;

        // ==== Atributos problema ==== //
        uint CityArea;               // Área de la ciudad que se desea cubrir.
        uint R;                      // Autonomia promedio de los vehiculos.
        uint num_nodes;              // Número de nodos disponibles, incluyendo nodos lejanos.
        uint size_poblation;         // Tamaño de la población.

        vector<node> nodes;          // Nodos no lejanos.
        vector<vector<bool>> matrix; // Matriz población.
        vector<uint> FEs;            // Función de evaluación de cada instancia en la población.
        vector<vector<uint>> dist;   // Distancia entre todos los nodos.

        /**
         * Operador cruzamiento entre 2 individuos.
         * @param pos: Posición donde se aplicara el cruzamiento.
         * @param i  : Indice padre 1 que se cruzara.
         * @param j  : Indice padre 2 que se cruzara.
         * @param i_p: Indice hijo 1.
         * @param j_p: Indice hijo 2.
         */
        void cruce(int pos,int i, int j, int i_p, int j_p) {
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
            bool rest1 = false; // Se asigno una estación de carga a un nodo lejano.
            bool rest2 = false; // La configuración actual es transitable.
            bool out = false;          // Retorna si se hizo un cambio.

            struct option {
                int index;
                float coef;
            };
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
            
            // Simular
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
            uint iteration = max_iterations;
            uint fe = 0;
            fe--; // Cómo es sin singo llega al valor más alto posible.
            for (int i = 0;i < num_nodes;i++) {
                uint temp = FE(i);
                if (temp < fe)
                    fe = temp;
            }

            while (0 < iteration--) {
            }
        }
};