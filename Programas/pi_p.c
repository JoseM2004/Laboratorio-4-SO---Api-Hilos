#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    long inicio;
    long fin;
    long n;
} HiloArgs;

double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static inline double f(double x) {
    return 4.0 / (1.0 + x * x);
}

void *calcular_suma_parcial(void *arg) {
    HiloArgs *datos = (HiloArgs *)arg;
    long   inicio = datos->inicio;
    long   fin = datos->fin;
    long   n = datos->n;
    double fH = 1.0 / (double)n;
 
    /* Suma local: no se necesita mutex aqui */
    double suma_local = 0.0;
    double fX;
    long   i;
 
    for (i = inicio; i < fin; i++) {
        fX = fH * ((double)i + 0.5);
        suma_local += f(fX);
    }
 
    /* Retornar la suma parcial al hilo principal */
    double *resultado = malloc(sizeof(double));
    if (!resultado) pthread_exit(NULL);
    *resultado = suma_local;
    pthread_exit((void *)resultado);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <n> <T>\n", argv[0]);
        fprintf(stderr, "  n = numero de rectangulos\n");
        fprintf(stderr, "  T = numero de hilos\n");
        return 1;
    }
 
    long n = atol(argv[1]);
    int  T = atoi(argv[2]);
 
    if (n <= 0 || T <= 0) {
        fprintf(stderr, "Error: n y T deben ser enteros positivos.\n");
        return 1;
    }
 
    printf("Calculando Pi con n = %ld rectangulos, T = %d hilos...\n", n, T);
 
    /* Reservar arreglos de hilos, argumentos y resultados */
    pthread_t *hilos  = malloc(T * sizeof(pthread_t));
    HiloArgs  *args   = malloc(T * sizeof(HiloArgs));
    if (!hilos || !args) {
        fprintf(stderr, "Error: no se pudo reservar memoria.\n");
        return 1;
    }
 
    /* Tamano del bloque de iteraciones por hilo */
    long bloque = n / T;
 
    double t_inicio = GetTime();
 
    /* Crear T hilos y asignar sub-rangos */
    for (int t = 0; t < T; t++) {
        args[t].n      = n;
        args[t].inicio = t * bloque;
        /* El ultimo hilo absorbe las iteraciones restantes */
        args[t].fin   = (t == T - 1) ? n : (t + 1) * bloque;
 
        int rc = pthread_create(&hilos[t], NULL, calcular_suma_parcial, &args[t]);
        if (rc != 0) {
            fprintf(stderr, "Error al crear hilo %d\n", t);
            return 1;
        }
    }
 
    /* Sincronizar y recolectar sumas parciales */
    double suma_total = 0.0;
    for (int t = 0; t < T; t++) {
        double *parcial = NULL;
        pthread_join(hilos[t], (void **)&parcial);
        if (parcial) {
            suma_total += *parcial;
            free(parcial);
        }
    }
 
    double t_fin = GetTime();
 
    /* Resultado final */
    double fH = 1.0 / (double)n;
    double pi  = fH * suma_total;
    double tiempo = t_fin - t_inicio;
 
    printf("Pi aproximado : %.15f\n", pi);
    printf("Pi (referencia): %.15f\n", acos(-1.0));
    printf("Error absoluto : %.2e\n",  fabs(pi - acos(-1.0)));
    printf("Tiempo (Tp)    : %.6f segundos\n", tiempo);
 
    free(hilos);
    free(args);
    return 0;
}