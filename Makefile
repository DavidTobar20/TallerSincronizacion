# Makefile para el Taller de Sincronizacion

# Compilador y flags
CC = gcc
CFLAGS = -Wall -pthread

# Detectar sistema operativo y ajustar flags de enlazado
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
    LDFLAGS = -lrt -lpthread  # Linux requiere ambas librerias
else
    LDFLAGS = -lpthread       # macOS solo necesita pthread
endif

# Ejecutables
TARGETS = producer consumer posixSincro

# Regla por defecto: compilar todos los programas
all: $(TARGETS)

# Regla generica: cada ejecutable depende de su .o correspondiente
%: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Regla generica para compilar archivos .c a .o
%.o: %.c shared.h
	$(CC) $(CFLAGS) -c $<

# Limpiar archivos objeto y ejecutables
# Borra semaforos y memoria compartida en /dev/shm
clean:
	rm -f *.o $(TARGETS)
	rm -f /dev/shm/sem.vacio /dev/shm/sem.lleno /dev/shm/memoria_compartida 
	
