#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <malloc.h>
#include <time.h>
int main()
{
    int *A, i, id, step = 0, n, Ns = 1, num_threads;
    printf("Nhap n = ");
    scanf("%d", &n);
    A = (int *)malloc((n + 1) * sizeof(int));
    srand(time(NULL));
    for (i = 0; i < n; i++)
    {
        *(A + i) = rand() % n;
        printf("%d ", *(A + i));
    }
    *(A + n) = 0;
    i = 0;
    clock_t start = clock();
    while (n > 1)
    {
        printf("\nStep %d:\n", ++i);
        Ns *= 2;
        num_threads = n / 2;
        n = (n + 1) / 2;
        omp_set_num_threads(num_threads);
#pragma omp parallel private(id)
        {
            id = omp_get_thread_num();
            *(A + id * Ns) = *(A + id * Ns) > *(A + id * Ns + Ns / 2) ? *(A + id * Ns) : *(A + id * Ns + Ns / 2);
            printf(" from thread %d: A[%d] = %d\n", id, id * Ns, *(A + id * Ns));
        }
    }
    clock_t end = clock();
    printf("\n Running time with parallel: %f, Max = %d", (double)(end - start) / CLOCKS_PER_SEC, *A);
    free(A);
    return 0;
}