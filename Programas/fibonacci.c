#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    long long *arreglo;
    int        N;
} FibArgs;

void *hilo_fibonacci(void *arg) {
    FibArgs   *datos   = (FibArgs *)arg;
    long long *arreglo = datos->arreglo;
    int        N       = datos->N;
 
    if (N <= 0) pthread_exit(NULL);
 
    arreglo[0] = 0;
    if (N > 1) arreglo[1] = 1;
 
    for (int i = 2; i < N; i++) {
        arreglo[i] = arreglo[i - 1] + arreglo[i - 2];
    }
 
    pthread_exit(NULL);
}

void calcular_fibonacci_serial(long long *arreglo, int N) {
    if (N <= 0) return;
    arreglo[0] = 0;
    if (N > 1) arreglo[1] = 1;
    for (int i = 2; i < N; i++) {
        arreglo[i] = arreglo[i - 1] + arreglo[i - 2];
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }
 
    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Error: N debe ser un entero positivo.\n");
        return 1;
    }
 
    /* Reservar arreglo compartido en el hilo principal */
    long long *arreglo = malloc(N * sizeof(long long));
    if (!arreglo) {
        fprintf(stderr, "Error: no se pudo reservar memoria para el arreglo.\n");
        return 1;
    }
 
    /* Preparar argumentos y lanzar el hilo trabajador */
    FibArgs   args   = { .arreglo = arreglo, .N = N };
    pthread_t worker;
 
    int rc = pthread_create(&worker, NULL, hilo_fibonacci, &args);
    if (rc != 0) {
        fprintf(stderr, "Error al crear el hilo trabajador.\n");
        free(arreglo);
        return 1;
    }
 
    /* Bloquear hasta que el trabajador termine (sincronizacion) */
    pthread_join(worker, NULL);
 
    /* Imprimir la secuencia generada por el hilo trabajador */
    printf("Secuencia de Fibonacci (%d elementos):\n", N);
    for (int i = 0; i < N; i++) {
        printf("F(%d) = %lld\n", i, arreglo[i]);
    }
 
    free(arreglo);
    return 0;
}