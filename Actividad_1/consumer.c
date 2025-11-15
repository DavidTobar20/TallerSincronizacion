/*********************************************************************************************
 * Pontificia Universidad Javeriana
 * Sistemas Operativos
 * Autor: Juan David Daza Caro
 * Docente: John Corredor PhD.
 * Taller de Sincronizacion - Problema Productor-Consumidor
 *
 * Descripción:
 * Proceso consumidor encargado de leer los elementos producidos por el proceso productor.
 * Ambos procesos se sincronizan usando semáforos POSIX con nombre y memoria compartida.
 *********************************************************************************************/

#include "shared.h"    // Cabecera que contiene la definición del buffer, estructura compartida
                      // y las librerías necesarias para semáforos + memoria compartida.

int main() {

    /* Abrir los semáforos creados previamente por el productor.
       - "vacio": indica cuántos espacios libres quedan en el buffer.
       - "lleno": indica cuántos elementos hay disponibles para consumir. */
    sem_t *vacio = sem_open("/vacio", 0);
    sem_t *lleno = sem_open("/lleno", 0);

    if (vacio == SEM_FAILED || lleno == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    /* Abrir el objeto de memoria compartida donde está el buffer.
       El productor ya lo creó, así que aquí solo se abre para lectura/escritura. */
    int fd_compartido = shm_open("/memoria_compartida", O_RDWR, 0644);
    if (fd_compartido < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /* Mapear la memoria compartida para poder usarla como una estructura normal.
       MAP_SHARED permite que los cambios sean visibles para ambos procesos. */
    compartir_datos *compartir = mmap(NULL, sizeof(compartir_datos),
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED,
                                      fd_compartido, 0);

    /* Inicializar el índice de salida. Desde aquí el consumidor comenzará a leer. */
    compartir->salida = 0;

    /* Bucle principal: consumir 10 elementos del buffer. */
    for (int i = 1; i <= 10; i++) {

        /* Esperar a que haya al menos un elemento en el buffer.
           Si "lleno" está en 0, el proceso queda bloqueado hasta que el productor escriba. */
        sem_wait(lleno);

        /* Leer el elemento actual del buffer circular. */
        int item = compartir->bus[compartir->salida];
        printf("Consumidor: Consume %d\n", item);

        /* Actualizar el índice circular de lectura. */
        compartir->salida = (compartir->salida + 1) % BUFFER;

        /* Avisar al productor que quedó un espacio libre más en el buffer. */
        sem_post(vacio);

        /* Simular tiempo de procesamiento del dato consumido. */
        sleep(2);
    }

    /* Liberar recursos usados por este proceso. */
    munmap(compartir, sizeof(compartir_datos));
    close(fd_compartido);
    sem_close(vacio);
    sem_close(lleno);

    return 0;
}
