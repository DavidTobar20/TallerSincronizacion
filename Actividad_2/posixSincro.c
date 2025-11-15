/*********************************************************************************************
 * Pontificia Universidad Javeriana
 * Sistemas Operativos
 * Taller de Sincronización - Hilos POSIX (pthreads)
 * Autor: Juan David Daza - David Tobar Artunduaga
 * Descripción:
 * Implementación del problema Productor–Consumidor usando hilos. 
 * Hay varios productores generando mensajes y un único consumidor.
 * La coordinación se hace con un mutex y variables de condición.
 *********************************************************************************************/
#include <stdio.h>      // printf, sprintf
#include <stdlib.h>     // exit, malloc si se necesitara
#include <pthread.h>    // hilos, mutex y variables de condición
#include <unistd.h>     // sleep
#include <string.h>     // manejo de cadenas

#define TAMANO_BUFFER 5                 // Tamaño del buffer circular
#define NUM_PRODUCTORES 10              // Cantidad de hilos productores
#define MENSAJES_POR_PRODUCTOR 10       // Mensajes que produce cada hilo

// Buffer donde se guardan los mensajes
char buffer[TAMANO_BUFFER][100];

// Índice donde el productor escribe un nuevo mensaje
int indice_escritura = 0;

// Índice donde el consumidor lee el siguiente mensaje
int indice_lectura = 0;

// Mutex para proteger acceso al buffer y contadores
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable de condición que indica que hay espacio libre
pthread_cond_t hay_espacio = PTHREAD_COND_INITIALIZER;

// Variable de condición que indica que hay mensajes disponibles
pthread_cond_t hay_datos   = PTHREAD_COND_INITIALIZER;

// Cantidad de espacios libres en el buffer
int espacios_disponibles = TAMANO_BUFFER;

// Cantidad de mensajes no consumidos aún
int mensajes_pendientes = 0;

// Prototipo del hilo productor
void *productor(void *arg);

// Prototipo del hilo consumidor
void *consumidor(void *arg);

int main() {
    pthread_t hilos_prod[NUM_PRODUCTORES];  // Arreglo de IDs de hilos productores
    pthread_t hilo_cons;                    // ID del hilo consumidor
    int ids[NUM_PRODUCTORES];               // IDs numéricos para los productores
    
    // Crear el hilo consumidor (se ejecuta en paralelo)
    pthread_create(&hilo_cons, NULL, consumidor, NULL);
    
    // Crear todos los hilos productores
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        ids[i] = i;                                     // Asignar ID del productor
        pthread_create(&hilos_prod[i], NULL, productor, &ids[i]); // Crear hilo
    }
    
    // Esperar a que todos los productores terminen su trabajo
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(hilos_prod[i], NULL);
    }
    
    // Esperar a que el consumidor procese los mensajes que quedaron
    while (mensajes_pendientes)
        sleep(1);
    
    // Detener al consumidor cuando no queda nada por procesar
    pthread_cancel(hilo_cons);

    return 0;
}

void *productor(void *arg) {
    int mi_id = *((int *) arg);     // ID numérico del hilo productor
    
    for (int i = 1; i <= MENSAJES_POR_PRODUCTOR; i++) {

        pthread_mutex_lock(&mutex); // Entrar a sección crítica
        
        // Si el buffer está lleno, esperar a que haya espacio
        while (espacios_disponibles == 0)
            pthread_cond_wait(&hay_espacio, &mutex);
        
        // Escribir el mensaje del productor en el buffer
        sprintf(buffer[indice_escritura], "Thread %d: %d\n", mi_id, i);
        
        // Avanzar el índice circular de escritura
        indice_escritura = (indice_escritura + 1) % TAMANO_BUFFER;

        // Actualizar contadores del estado del buffer
        espacios_disponibles--;
        mensajes_pendientes++;
        
        // Avisar al consumidor que ahora sí hay datos disponibles
        pthread_cond_signal(&hay_datos);
        
        pthread_mutex_unlock(&mutex); // Salir de sección crítica
        
        sleep(1);  // Simular trabajo del productor
    }
    
    return NULL;
}

void *consumidor(void *arg) {
    (void)arg; // Evitar advertencia por parámetro sin usar
    
    while (1) {
        pthread_mutex_lock(&mutex); // Entrar a sección crítica
        
        // Si no hay mensajes, esperar a que un productor produzca alguno
        while (mensajes_pendientes == 0)
            pthread_cond_wait(&hay_datos, &mutex);
        
        // Leer el mensaje que corresponde en el índice de lectura
        printf("%s", buffer[indice_lectura]);
        
        // Avanzar índice circular de lectura
        indice_lectura = (indice_lectura + 1) % TAMANO_BUFFER;

        // Actualizar contadores
        mensajes_pendientes--;
        espacios_disponibles++;
        
        // Avisar a los productores que hay un espacio libre disponible
        pthread_cond_signal(&hay_espacio);

        pthread_mutex_unlock(&mutex); // Salir de sección crítica
    }
    
    return NULL;
}
