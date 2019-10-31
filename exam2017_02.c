#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#define CHUNKSIZE 2
#define OMP_NUM_THREADS 4

void monte_carlo(int n,
                 int *seed);
double random_value(int *seed);

int main(int argc, char *argv[])
{
    omp_set_num_threads(OMP_NUM_THREADS);
    
    int n;
    int seed;
    printf(" Number of processors available = %d\n",
           omp_get_num_procs());
    printf(" Number of threads = %d\n",
           omp_get_max_threads());

    n = 10;
    seed = 1234567890;
    monte_carlo(n,
                &seed);
    printf(" Normal end of execution.\n");
    return 0;
}

void monte_carlo(int n,
                 int *seed)
{
    int i;
    int my_id;
    int my_seed;
    int chunk = CHUNKSIZE;
    double *x;

    x = (double *)malloc(n * sizeof(double));

#pragma omp master
    {
        printf("\n");
        printf(" Thread Seed I X(I)\n");
        printf("\n");
    }
#pragma omp parallel private(i, my_id, my_seed) shared(n, x)
    {
        my_id = omp_get_thread_num();
        my_seed = *seed^my_id;
        printf(" %6d %12d\n", my_id, my_seed);

#pragma omp for schedule(dynamic, chunk)
        for (i = 0; i < n; i++)
        {
            x[i] = random_value(&my_seed);
            printf(" %6d %6d %14.6g\n", my_id, i, x[i]);
        }
    }
    free(x);
    return;
}

double random_value(int *seed)
{
    double r;
    *seed = (*seed % 65536);
    *seed = ((3125 * *seed) % 65536);
    r = (double)(*seed) / 65536.0;
    return r;
}