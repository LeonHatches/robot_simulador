#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <string>
#include <iomanip>
#include <ctime>

using namespace std;

struct Particula {
    double x;
    double peso;
    double vel;
};

// Definir variables globales
const int N = 1000;
const double LIMITE = 100.0;

// Configuración inicial
default_random_engine gen(random_device{}());
double velocidad = 1.0;

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

// Movimiento de las particulas
void moverParticulas(vector<Particula> &particulas, double ruidoMov, double limite) {
    for (auto &p : particulas) {
        double ruido = ((double)rand() / RAND_MAX) * (2 * ruidoMov) - ruidoMov;
        p.x = p.x + p.vel + ruido;

        if (p.x < 0) p.x = 0;
        if (p.x > limite) p.x = limite;
    }
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

// Normalización (Filtro de partículas)
void normalizarParticulas(vector<Particula> &particulas) {
    double sum = 0;
    for (auto &particula : particulas)
        sum += particula.peso;

    for (auto &particula : particulas) {
        particula.peso /= sum;
    }
};

// Actualización de pesos (verosimilitud + normalización)
void actualizarPesos(vector<Particula> &particulas,
                    double medicion,
                    double pared,
                    double ruidoSensor) 
{
    const double minPeso = 1e-300;

    for (auto &p : particulas) {
        double esperado = pared - p.x;
        double error = medicion - esperado;

        double prob = gaussianPdf(error, ruidoSensor);

        p.peso *= prob;
        if (p.peso < minPeso)
            p.peso = minPeso;
    }

    normalizarParticulas(particulas);
}

//  Remuestreo de particulas
vector<Particula> remuestrearParticulas(vector<Particula> &particulas) {
    int n = particulas.size();
    vector<Particula> nParticulas(n);

    vector<double> pesos(n);
    for (int i = 0; i < n; i++)
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

        nParticulas[i].x   = particulas[index].x;
        nParticulas[i].peso = 1.0 / n;
        nParticulas[i].vel  = particulas[index].vel;
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
int main() {
    srand(time(NULL));

    // Configuracion inicial
    const int pasosTotales = 100;  
    const double pared = LIMITE;   
    const double ruidoMovimiento = 0.1;
    const double ruidoSensor = 0.1;
    const string csvPath = "datos_simulacion.csv";

    // Crear robot real
    Robot robot;
    robot.x = 0.0;
    robot.velocidad = velocidad;
    robot.ruidoMov = ruidoMovimiento;

    cout << " Simulacion filtro de particulas 1D " << endl;
    cout << "Partículas: " << N << " | Limite: " << LIMITE << endl;
    cout << "------------------------------------------" << endl;

    // Inicializar particulas
    vector<Particula> particulas = inicializarParticulas();
    cout << "Particulas inicializadas correctamente.\n";      

    // Limpiar archivo CSV previo
    ofstream limpiar(csvPath, ios::trunc);
    limpiar.close();

    cout << "\n Inicio de la simulacion \n";

    double errorAcumulado = 0.0;
    double errorMaximo = 0.0;
    double errorMinimo = 1e9;

    // bucle principal de simulación
    for (int paso = 0; paso < pasosTotales; ++paso) {

        // 1. Movimiento real del robot
        moverRobot(robot, LIMITE);

        // 2. Movimiento de partículas (modelo predictivo)
        moverParticulas(particulas, ruidoMovimiento, LIMITE);

        // 3. Medición del sensor
        double medicion = medirDistancia(robot, pared, ruidoSensor);

        // 4. Actualización de pesos según verosimilitud
        actualizarPesos(particulas, medicion, pared, ruidoSensor);

        // 5. Remuestreo
        particulas = remuestrearParticulas(particulas);

        // 6. Estimación y registro en CSV
        double est = estimarPosicionYExportar(particulas, robot.x, csvPath, paso);

        // 7. Mostrar en consola
        cout << "Paso " << setw(3) << paso
             << " | Real: " << setw(8) << fixed << setprecision(4) << robot.x
             << " | Est: "  << setw(8) << est
             << " | Error: " << setw(8) << fabs(est - robot.x)
             << endl;

        // estadísticas
        double errorPaso = fabs(est - robot.x);
        errorAcumulado += errorPaso;
        if (errorPaso > errorMaximo) errorMaximo = errorPaso;
        if (errorPaso < errorMinimo) errorMinimo = errorPaso;             
    }

    cout << "\n Fin de simulacion " << endl;
    cout << "Datos exportados a: " << csvPath << endl;
    double errorPromedio = errorAcumulado / pasosTotales;

    cout << "\nResumen final\n";
    cout << "Error promedio: " << errorPromedio << endl;
    cout << "Error máximo:   " << errorMaximo << endl;
    cout << "Error mínimo:   " << errorMinimo << endl;
    
    return 0;
}
