#include <iostream>
#include <vector>
#include <numeric>
#include <random>
// Incluir

using namespace std;

struct Particula {
    double x;
    double peso;
};

// Definir variables globales


// Configuración inicial


// Cabecera


// Moviviento real del robot


// Sensor de profundidad


// Incializar partículas


// Función de verosimilitud


// Normalización (Filtro de partículas)
void normalizarParticulas(vector<Particula> &particulas) {
    double sum = 0;
    for (auto &particula : particulas)
        sum += particula.peso;

    for (auto &particula : particulas) {
        particula.peso /= sum;
    }
};

//  Remuestreo de particulas
vector<Particula> remuestrearParticulas(vector<Particula> &particulas) {
    int n = particulas.size();
    vector<Particula> nParticulas(n);

    vector<double> pesos(n);
    for (size_t i = 0; i < particulas.size(); i++)
        pesos[i] = particulas[i].peso;

    vector<double> sumaAcumulada(n);
    partial_sum(pesos.begin(), pesos.end(), sumaAcumulada.begin());

    default_random_engine re(random_device{}());
    uniform_real_distribution<double> random(0.0, 1.0 / n);

    double inicio = random(re);
    double paso = 1.0 / n;
    int index = 0;

    for (int i = 0; i < n; i++) {
        double u = inicio + i * paso;

        while (u > sumaAcumulada[index] && index < n - 1)
            index++;

        nParticulas[i].x = particulas[index].x;
        nParticulas[i].peso = 1.0 / n;
    }

    return nParticulas;
}

// Estimación de posición
void printParticulas(vector<Particula> particulas) {
    for (size_t i = 0; i < particulas.size(); i++)
        cout << "[" << i << "] : {" << particulas[i].x << ", " << particulas[i].peso << "}\n";
}

// Main
int main() {
    vector<Particula> particulas = {
        {1.0, 0.05}, {2.5, 0.12}, {3.1, 0.08}, {4.0, 0.15}, {5.2, 0.03},
        {6.8, 0.20}, {7.1, 0.09}, {8.5, 0.11}, {9.0, 0.07}, {0.3, 0.18},
        {1.5, 0.06}, {2.2, 0.10}, {3.9, 0.13}, {4.4, 0.04}, {5.0, 0.17},
        {6.1, 0.02}, {7.7, 0.16}, {8.8, 0.01}, {9.5, 0.14}, {0.9, 0.19}
    };

    printParticulas(particulas);

    cout << "\n";

    normalizarParticulas(particulas);
    printParticulas(particulas);

    cout << "\n";

    particulas = remuestrearParticulas(particulas);
    printParticulas(particulas);
}
