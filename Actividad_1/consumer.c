/*********************************************************************************************
 * Pontificia Universidad Javeriana
 * Sistemas Operativos
 * Autor: Juan David Daza Caro
 * Docente: John Corredor PhD.
 * Taller de Sincronizacion - Problema Productor-Consumidor
 * 
 * Descripcion:
 * Proceso consumidor que extrae items del buffer compartido.
 * Utiliza semaforos con nombre para sincronizacion entre procesos. (Posix)
 *********************************************************************************************/

#include "shared.h"

int main() {
    // Abrir semaforos existentes creados por el productor
    sem_t *vacio = sem_open("/vacio", 0);
    sem_t *lleno = sem_open("/lleno", 0);
    
    if (vacio == SEM_FAILED || lleno == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Abrir la region de memoria compartida existente
    int fd_compartido = shm_open("/memoria_compartida", O_RDWR, 0644); // Permisos de lectura/escritura
    if (fd_compartido < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria compartida al espacio de direcciones del proceso
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos), PROT_READ | PROT_WRITE, MAP_SHARED, fd_compartido, 0);
    
    // Inicializar el indice de salida
    compartir->salida = 0;

    // Consumir 10 items
    for (int i = 1; i <= 10; i++) {
        // Esperar si no hay items disponibles (buffer vacio)
        sem_wait(lleno);
        
        // Extraer item del buffer
        int item = compartir->bus[compartir->salida];
        printf("Consumidor: Consume %d\n", item);
        
        // Actualizar indice de salida de forma circular
        compartir->salida = (compartir->salida + 1) % BUFFER;
        
        // Señalar que hay un nuevo espacio vacio disponible
        sem_post(vacio);
        
        // Simular tiempo de consumo
        sleep(2);  
    }

    // Liberar recursos
    munmap(compartir, sizeof(compartir_datos));
    close(fd_compartido);
    sem_close(vacio);
    sem_close(lleno);
    
    return 0;
}
