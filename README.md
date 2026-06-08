# Práctica No. 4 — API de Hilos (Pthreads)
**Laboratorio de Sistemas Operativos · Ingeniería de Sistemas · Universidad de Antioquia**

---

## Integrantes

| Nombre completo | Correo | N° Documento |
|-----------------|--------|--------------|
| **Jose Miguel Monsalve Marin** | **jmiguel.monsalve@udea.edu.co** | **1038062019** |
| **Miguel Angel Agudelo Vera** | **miguel.agudelo2@udea.edu.co** | **1039678287** |

---


## Compilación

```bash
# Versión serial de π
gcc -O2 -o pi_s src/pi.c -lm

# Versión paralela de π
gcc -O2 -o pi_p src/pi_p.c -lpthread -lm

# Fibonacci
gcc -O2 -o fibonacci src/fibonacci.c -lpthread
```

---

## Ejecución

```bash
# Serial (n = número de rectángulos)
./pi_s 2000000000

# Paralelo (n = rectángulos, T = hilos)
./pi_p 2000000000 4

# Fibonacci (N = cantidad de números)
./fibonacci 15
```

---

## Documentación de funciones

### `pi.c` — Versión serial

| Función | Descripción |
|---------|-------------|
| `double GetTime(void)` | Retorna el tiempo actual en segundos usando `CLOCK_MONOTONIC`. Resolución de nanosegundos. |
| `double f(double x)` | Evalúa el integrando `4 / (1 + x²)`. Declarada `inline` para reducir overhead. |
| `double CalcPi(long n)` | Aproxima π usando la regla del punto medio con `n` rectángulos. Retorna la aproximación. |
| `int main(int argc, char *argv[])` | Lee `n` de la línea de comandos, mide el tiempo de `CalcPi` e imprime el resultado. |

### `pi_p.c` — Versión paralela

| Función / Tipo | Descripción |
|----------------|-------------|
| `HiloArgs` | Estructura con campos `inicio`, `fin` y `n` para delimitar el sub-rango de cada hilo. |
| `double GetTime(void)` | Igual a la versión serial. |
| `double f(double x)` | Igual a la versión serial. |
| `void *calcular_suma_parcial(void *arg)` | Función del hilo trabajador. Itera sobre el sub-rango `[inicio, fin)`, acumula la suma en una variable local y la retorna mediante `pthread_exit` en un `double` reservado en el heap. |
| `int main(int argc, char *argv[])` | Lee `n` y `T`; crea `T` hilos con sub-rangos balanceados; sincroniza con `pthread_join`; agrega sumas parciales; imprime π y el tiempo. |

### `fibonacci.c`

| Función / Tipo | Descripción |
|----------------|-------------|
| `FibArgs` | Estructura con puntero `arreglo` (memoria compartida) y entero `N`. |
| `void *hilo_fibonacci(void *arg)` | Hilo trabajador: rellena el arreglo compartido con los primeros `N` valores de Fibonacci y termina con `pthread_exit(NULL)`. |
| `void calcular_fibonacci_serial(long long *arreglo, int N)` | Versión sin hilos para comparación de tiempos en el análisis. |
| `int main(int argc, char *argv[])` | Lee `N`; reserva el arreglo con `malloc`; lanza el hilo trabajador; espera con `pthread_join`; imprime la secuencia; libera la memoria. |

---

## Problemas encontrados y soluciones

| Problema | Solución |
|----------|----------|
| **Retorno de valor desde el hilo**: `pthread_exit` solo admite `void *`, no un `double` directamente. | Se reserva un `double` en el heap con `malloc` dentro del hilo y se pasa el puntero. El hilo main lo recupera vía `pthread_join` y lo libera con `free`. |
| **Iteraciones sobrantes**: con `n` no divisible exactamente entre `T`, algún hilo quedaría sin las últimas iteraciones. | El último hilo (índice `T-1`) recibe como `fin` directamente `n`, absorbiendo el resto. |
| **Condición de carrera en Fibonacci**: main podría leer el arreglo antes de que el trabajador termine. | `pthread_join` garantiza la barrera de sincronización: main solo accede al arreglo después de que `join` retorna. |
| **Overhead con 1 hilo**: `T_p(1) > T_s`. | Es esperado: `pthread_create` y `pthread_join` tienen costo de sistema. Se documenta en el análisis. |

---

## Pruebas realizadas

### pi_s / pi_p
```bash
$ ./pi_s 1000000
```
<img width="598" height="97" alt="image" src="https://github.com/user-attachments/assets/4519ec1f-ba32-4e52-9caa-b3cfbbfe6b8f" />

```bash
$ ./pi_p 1000000 4
```
<img width="620" height="135" alt="image" src="https://github.com/user-attachments/assets/9ac7341b-ae72-456a-8e84-c384d03b862f" />

```text
Ambas versiones convergen al mismo valor de π
```

### fibonacci
```bash
./fibonacci 10
F(0)=0  F(1)=1  F(2)=1  F(3)=2  F(4)=3
F(5)=5  F(6)=8  F(7)=13  F(8)=21  F(9)=34
```
<img width="597" height="246" alt="image" src="https://github.com/user-attachments/assets/12f9c7d9-a075-44e2-8546-2fc7cc1bb80f" />

```bash
./fibonacci 1
F(0)=0     # caso borde N=1
```

<img width="597" height="67" alt="image" src="https://github.com/user-attachments/assets/b63774f3-4b5d-43a9-bb18-099a73ae739c" />

```bash
./fibonacci 15
F(0)=0  F(1)=1  F(2)=1  F(3)=2  F(4)=3
F(5)=5  F(6)=8  F(7)=13  F(8)=21  F(9)=34
F(10)=55  F(11)=89  F(12)=144  F(33)=233  F(14)=377

```

<img width="586" height="372" alt="image" src="https://github.com/user-attachments/assets/5b06f83c-090e-4495-87d9-6fdedc0884f8" />

---

## Video

> 🎥 [Enlace al video de sustentación (10 min)](https://youtu.be/XXXXXXXXXXXXX) *(reemplazar con el enlace real)*

---

## Manifiesto de Transparencia — Uso de IA Generativa

En esta práctica se usó IA generativa (Claude, Anthropic) en los siguientes puntos:

1. **Estructura inicial de `pi_p.c`**: se solicitó una guía sobre el patrón de retorno de valores desde hilos POSIX mediante `pthread_exit` + puntero a heap.
2. **Revisión de condiciones de borde**: la IA sugirió que el último hilo tomara `fin = n` para evitar iteraciones perdidas.
3. **Redacción del análisis**: se usó la IA para organizar las ideas sobre eficiencia, Ley de Amdahl y overhead de contexto.
4. **Notebook**: la estructura de la tabla y las gráficas de speedup/eficiencia fue asistida por la IA.

Toda la implementación fue revisada, comprendida y verificada manualmente por nosotros.

---

## Conclusiones

1. **La paralelización con Pthreads escala bien para cargas CPU-bound hasta el límite del hardware físico**: el cálculo de π permitió alcanzar un speedup de ~3.89× con 8 hilos, cercano al número de núcleos físicos del i5-10300H (4 núcleos), confirmando que este tipo de carga se beneficia directamente del paralelismo real disponible en la máquina.

2. **El overhead de creación de hilos es real pero pequeño**: T_p(1) = 10.6969 s fue ~1.5% más lento que T_s = 10.5334 s (~163 ms de diferencia), lo que confirma que la API de Pthreads tiene un costo fijo por hilo que se amortiza cuando el workload es suficientemente grande (como n = 2 000 000 000).

3. **La eficiencia colapsa al sobrepasar el número de núcleos lógicos**: la caída más drástica ocurre entre T=8 (eficiencia 0.49) y T=16 (eficiencia 0.22). Con 16 hilos compitiendo por 8 núcleos lógicos, cada hilo aprovecha menos del 25% del recurso que le corresponde, demostrando que la sobresuscripción es más perjudicial que beneficiosa.

4. **El modelo de memoria compartida simplifica la comunicación entre hilos**: en Fibonacci, el arreglo compartido en el heap elimina la necesidad de copiar datos, reduciendo tanto la complejidad del código como el uso de memoria.

5. **`pthread_join` es imprescindible para la corrección del programa**: sin él, la impresión de la secuencia podría adelantarse a la escritura del trabajador, produciendo valores erróneos o no inicializados (race condition).

6. **Evitar mutex dentro del bucle es clave para el rendimiento**: la estrategia de sumas parciales locales elimina la contención de mutex en el camino crítico, que de otro modo sería el cuello de botella principal en la versión paralela.

7. **La Ley de Amdahl limita el speedup teórico**: incluso con infinitos hilos, la fracción serial del programa (creación de hilos, agregación de resultados, I/O) pone un techo al speedup máximo alcanzable. En este experimento ese techo se hace evidente: pasar de 8 a 16 hilos no solo no mejora, sino que reduce el speedup de 3.89 a 3.59.

---

## Analisia Notebook
[Notebook Google Colab analisis.ipynb](Programas/analisis.ipynb)
