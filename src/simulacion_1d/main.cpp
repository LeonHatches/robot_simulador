#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <string>
#include <iomanip>
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
//para pruebas
//const int N = 1000;
//Mas grande, cuidado con la ram
//const long long N = 1000000;
//const long long N = 100000000;

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
void moverRobot(Robot &r, double limite) {
    double ruido = ((double)rand() / RAND_MAX) * (2 * r.ruidoMov) - r.ruidoMov;
    r.x = r.x + r.velocidad + ruido;
    if (r.x < 0) r.x = 0;
    if (r.x > limite) r.x = limite;
}

// Sensor de profundidad
double medirDistancia(const Robot &r, double pared, double ruidoSensor) {
    double distanciaReal = pared - r.x;
    double ruido = ((double)rand() / RAND_MAX) * (2 * ruidoSensor) - ruidoSensor;
    double medicion = distanciaReal + ruido;

    if (medicion < 0) medicion = 0;
    
    return medicion;
}


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
static double gaussianPdf(double x, double sigma) {
    if (sigma <= 0.0) return (std::abs(x) < 1e-12) ? 1.0 : 0.0;

    const double PI = acos(-1.0);
    double denom = sigma * sqrt(2.0 * PI);
    double expo  = -0.5 * (x * x) / (sigma * sigma);
    return exp(expo) / denom;
}

void actualizarPesos(vector<Particula> &particulas,
                    double medicion,
                    double pared,
                    double ruidoSensor) 
{
    const double minPeso = 1e-300;

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
// Calcula la posición estimada como promedio ponderado de las partículas
// y exporta (apend) una línea CSV con: paso, posicion_real, posicion_estimada, error
// Retorna la posición estimada.
double estimarPosicionYExportar(const std::vector<Particula> &particulas,
                                double posicionReal,
                                const std::string &csvPath,
                                int paso) {
    double numerador = 0.0;
    double denominador = 0.0;

    for (const auto &p : particulas) {
        numerador += p.x * p.peso;
        denominador += p.peso;
    }

    double estimada = 0.0;
    if (denominador > 0.0)
        estimada = numerador / denominador;

    double error = std::fabs(estimada - posicionReal);

    std::ofstream out(csvPath, std::ios::app);
    if (!out) {
        std::cerr << "No se pudo abrir el archivo CSV: " << csvPath << std::endl;
        return estimada;
    }

    // Si es el primer paso (0) añadimos cabecera
    if (paso == 0) {
        out << "paso,posicion_real,posicion_estimada,error" << '\n';
    }

    out << paso << ',' << std::fixed << std::setprecision(6)
        << posicionReal << ',' << estimada << ',' << error << '\n';

    out.close();
    return estimada;
}

// Main
