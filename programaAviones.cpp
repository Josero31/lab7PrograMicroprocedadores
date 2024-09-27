#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>


pthread_mutex_t pista_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gates_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pista_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t gates_cond = PTHREAD_COND_INITIALIZER;

bool pista_disponible = true;
int gates_disponibles = 2;

void log(const std::string &mensaje) {
    std::cout << mensaje << std::endl;
}

void* avion(void* id) {
    int avion_id = *((int*)id);
    log("Avión " + std::to_string(avion_id) + " solicitando permiso para aterrizar.");

    // Solicitar pista
    pthread_mutex_lock(&pista_mutex);
    while (!pista_disponible) {
        pthread_cond_wait(&pista_cond, &pista_mutex);
    }
    pista_disponible = false;
    log("Avión " + std::to_string(avion_id) + " aterrizando.");
    pthread_mutex_unlock(&pista_mutex);

    // Simular aterrizaje
    sleep(1);

    // Liberar pista
    pthread_mutex_lock(&pista_mutex);
    pista_disponible = true;
    pthread_cond_signal(&pista_cond);
    pthread_mutex_unlock(&pista_mutex);

    // Solicitar gate
    pthread_mutex_lock(&gates_mutex);
    while (gates_disponibles == 0) {
        pthread_cond_wait(&gates_cond, &gates_mutex);
    }
    gates_disponibles--;
    log("Avión " + std::to_string(avion_id) + " ocupando un gate.");
    pthread_mutex_unlock(&gates_mutex);

    // Simular desembarque
    sleep(2);

    // Liberar gate
    pthread_mutex_lock(&gates_mutex);
    gates_disponibles++;
    pthread_cond_signal(&gates_cond);
    pthread_mutex_unlock(&gates_mutex);

    log("Avión " + std::to_string(avion_id) + " ha liberado el gate.");
    return nullptr;
}

int main() {
    pthread_t aviones[4];
    int ids[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; ++i) {
        pthread_create(&aviones[i], nullptr, avion, &ids[i]);
    }

    for (int i = 0; i < 4; ++i) {
        pthread_join(aviones[i], nullptr);
    }

    return 0;
}
