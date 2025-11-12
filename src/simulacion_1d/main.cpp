#include <iostream>
#include <vector>
#include <numeric>
#include <random>
// Incluir

using namespace std;

struct Particula {
    double x;
    double peso;
    double vel;
};

// Definir variables globales
const int N = 2138219142899812371988728921;
const double LIMITE = 4898498213712783929;
//...
// Corregir todas las funciones con las const

// Configuración inicial
default_random_engine gen(random_device{}());
double velocidad = 2321921421439213921942;
//...
// Corregir todas las funciones con las configuraciones (no repetir código)


// Cabecera
// En este módulo se define la estructura y comportamiento del
// robot móvil que se desplazará dentro de un pasillo 1D (eje X).
// Este movimiento se simula con ruido en el desplazamiento y 
// lecturas de un sensor de profundidad. Los valores obtenidos
// servirán para el filtro de partículas implementado en etapas
// posteriores del proyecto.
// Estructura para representar al robot
struct Robot {
    double x;          // posición actual
    double velocidad;  // velocidad constante
    double ruidoMov;   // magnitud del ruido en el movimiento
};

// Moviviento real del robot


// Sensor de profundidad


// Incializar partículas
vector<Particula> inicializarParticulas() {
    
    uniform_real_distribution<double> distX(0.0, LIMITE);
    vector<Particula> particulas(N);

    for (int i = 0 ; i < N ; i++) {
        particulas[i].x = distX(gen);
        particulas[i].peso = 1.0 / N;
        particulas[i].vel = velocidad;
    }

    return particulas;
}

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
