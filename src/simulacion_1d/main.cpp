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

// Main
