# Taller de Sincronizacion - Sistemas Operativos

Este taller implementa el problema clasico productor-consumidor usando dos enfoques diferentes de sincronizacion en sistemas POSIX.

## Actividad 1: Productor-Consumidor con Procesos

Implementa el problema productor-consumidor usando procesos independientes que se comunican mediante:
- Semaforos con nombre (`sem_open`, `sem_wait`, `sem_post`)
- Memoria compartida (`shm_open`, `mmap`)
- Buffer circular de 5 espacios

El productor genera 10 items (1 por segundo) y el consumidor los consume (1 cada 2 segundos).

### Ejecucion:

```bash
# Compilar
make

# Ejecutar productor (debe ir primero para crear recursos compartidos)
# El ampersand es un operador de bash, se usa para que el programa se ejecute en segundo plano
./producer &

# Ejecutar consumidor en otra terminal
./consumer
```

## Actividad 2: Productor-Consumidor con Hilos

Implementa el mismo problema usando hilos POSIX dentro de un solo proceso:
- 10 hilos productores que generan mensajes concurrentemente
- 1 hilo consumidor que los imprime
- Mutex (`pthread_mutex`) para exclusion mutua
- Variables de condicion (`pthread_cond`) para sincronizacion

Cada productor genera 10 mensajes, resultando en 100 mensajes totales.

### Ejecucion:

```bash
# Compilar
make

# Ejecutar
./posixSincro
```

## Comandos del Makefile

```bash
make              # Compila todos los programas
make clean        # Elimina archivos compilados y recursos compartidos
```
