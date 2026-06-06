#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct node {
    int id;  // ID del nodo.
    int f_i; // Capacidad de carga del nodo(si se coloca un cargador).
    int c_i; // Costo asociado a colocar un punto de carga en el nodo.
};

class AE {
    private:
        int alpha;                   // Probabilidad de una mutación.
        int max_iteration;           // Maximas iteraciones permitidas.

        int num_nodes;               // Número de nodos disponibles, incluyendo nodos lejanos.
        int size_poblation;          // Tamaño de la población.

        vector<node> nodes;          // Nodos no lejanos.
        vector<node> far_nodes;      // Nodos leganos.
        vector<vector<bool>> matrix; // Matriz población.
        vector<int> FEs;             // Función de evaluación de cada instancia en la población.
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

        }
        /**
         * Función que mide la calidad de una solución particular.
         * @param i: Individuo de la población que se quiere evaluar.
         * @return Costo asociado a una instancia en particular.
         */
        int FE(int i) {
            // ...
        }
        // Función que resuelve el problema usando Algoritmo Evolutivo.
        void solve() {
            // ...
        }
};
