#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

double CalcPi(long n) {
    const double fH  = 1.0 / (double)n;
    double       fSum = 0.0;
    double       fX;
    long         i;
 
    for (i = 0; i < n; i++) {
        fX    = fH * ((double)i + 0.5);
        fSum += f(fX);
    }
    return fH * fSum;
}

int main(int argc, char *argv[]) {
    long n = 2000000000L;   /* valor por defecto */
    struct timespec t0, t1;
 
    if (argc >= 2) {
        n = atol(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "Error: n debe ser un entero positivo.\n");
            return 1;
        }
    }
 
    printf("Calculando Pi con n = %ld rectangulos (serial)...\n", n);
    double t_inicio = GetTime();
    double pi = CalcPi(n);
    double t_fin = GetTime();
 
    double tiempo = t_fin - t_inicio;
 
    printf("Pi aproximado : %.15f\n", pi);
    printf("Tiempo (Ts)    : %.6f segundos\n", tiempo);
    return 0;
}
