#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

using namespace std;

// Recursos disponibles
int impresoras = 2;
int escaneres = 1;
int plotter = 1;

// Mutex y variables de condición
mutex mtx;
condition_variable cv;

// Función para intentar adquirir recursos
bool adquirirRecursos(int &imp, int &esc, int &plo, int impSolicitadas, int escSolicitadas, int ploSolicitados) {
    unique_lock<mutex> lock(mtx);

    // Esperar hasta que los recursos solicitados estén disponibles
    cv.wait(lock, [&] {
        return impresoras >= impSolicitadas && escaneres >= escSolicitadas && plotter >= ploSolicitados;
    });

    // Adquirir los recursos
    impresoras -= impSolicitadas;
    escaneres -= escSolicitadas;
    plotter -= ploSolicitados;

    imp = impSolicitadas;
    esc = escSolicitadas;
    plo = ploSolicitados;

    return true;
}

// Función para liberar recursos
void liberarRecursos(int imp, int esc, int plo) {
    unique_lock<mutex> lock(mtx);
    impresoras += imp;
    escaneres += esc;
    plotter += plo;
    cv.notify_all();  // Despertar a los departamentos esperando por recursos
}

// Función para simular el trabajo de los departamentos
void departamentoTrabajo(const string &nombre, int impSolicitadas, int escSolicitadas, int ploSolicitados) {
    int imp = 0, esc = 0, plo = 0;

    while (!adquirirRecursos(imp, esc, plo, impSolicitadas, escSolicitadas, ploSolicitados)) {
        cout << nombre << " esperando por recursos...\n";
        this_thread::sleep_for(chrono::milliseconds(1000));  // Espera antes de reintentar
    }

    cout << nombre << " adquirió " << imp << " impresoras, " << esc << " escáneres, y " << plo << " plotters.\n";

    // Simular el uso de los recursos por un tiempo
    this_thread::sleep_for(chrono::seconds(2));

    cout << nombre << " terminó su trabajo.\n";

    // Liberar los recursos
    liberarRecursos(imp, esc, plo);
}

int main() {
    // Crear hilos para los departamentos
    thread d1(departamentoTrabajo, "Departamento 1", 1, 1, 0);
    thread d2(departamentoTrabajo, "Departamento 2", 1, 0, 1);
    thread d3(departamentoTrabajo, "Departamento 3", 1, 0, 1);

    // Esperar a que los hilos terminen
    d1.join();
    d2.join();
    d3.join();

    return 0;
}
