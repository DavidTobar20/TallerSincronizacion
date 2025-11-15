/*********************************************************************************************
 * Pontificia Universidad Javeriana
 * Sistemas Operativos
 * Taller de Sincronizacion - Problema Productor-Consumidor
 * Autor: Juan David Daza - David Tobar Artunduaga
 * Descripcion:
 * Proceso productor que genera items y los coloca en un buffer compartido.
 * Utiliza semaforos con nombre para sincronizacion entre procesos.
 *********************************************************************************************/

#include "shared.h"

int main() {
    // Crear o abrir semaforo "vacio" inicializado con BUFFER espacios disponibles
    sem_t *vacio = sem_open("/vacio", O_CREAT, 0644, BUFFER);
    
    // Crear o abrir semaforo "lleno" inicializado en 0 (buffer vacio al inicio)
    sem_t *lleno = sem_open("/lleno", O_CREAT, 0644, 0);
    
    if (vacio == SEM_FAILED || lleno == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Crear o abrir region de memoria compartida
    int shm_fd = shm_open("/memoria_compartida", O_CREAT | O_RDWR, 0644);
    if (shm_fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    // Establecer el tamano de la memoria compartida
    ftruncate(shm_fd, sizeof(compartir_datos));

    // Mapear la memoria compartida al espacio de direcciones del proceso
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos), 
                                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    
    // Inicializar el indice de entrada
    compartir->entrada = 0;

    // Producir 10 items
    for (int i = 1; i <= 10; i++) {
        // Esperar si no hay espacios vacios (buffer lleno)
        sem_wait(vacio);
        
        // Seccion critica: insertar item en el buffer
        compartir->bus[compartir->entrada] = i;
        printf("Productor: Produce %d\n", i);
        
        // Actualizar indice de entrada de forma circular
        compartir->entrada = (compartir->entrada + 1) % BUFFER;
        
        // Senalar que hay un nuevo item disponible
        sem_post(lleno);
        
        // Simular tiempo de produccion
        sleep(1);  
    }

    // Liberar recursos
    munmap(compartir, sizeof(compartir_datos));
    close(shm_fd);
    sem_close(vacio);
    sem_close(lleno);
    
    // Limpiar semaforos y memoria compartida
    sem_unlink("/vacio");
    sem_unlink("/lleno");
    shm_unlink("/memoria_compartida");
    
    return 0;
}
