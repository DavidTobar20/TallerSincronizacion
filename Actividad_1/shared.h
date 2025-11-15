// Cabecera compartida entre productor y consumidor


#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Definicion del tamano del buffer compartido
#define BUFFER 5

// Estructura para compartir datos entre procesos
typedef struct {
    int bus[BUFFER];  // Buffer circular para almacenar items
    int entrada;      // Indice donde el productor inserta items
    int salida;       // Indice donde el consumidor extrae items
} compartir_datos;

#endif 
