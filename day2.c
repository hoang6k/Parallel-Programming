#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <malloc.h>
#include <time.h>
int main()
{
    srand(0);
    int i, j, k, m, n, p, id, num_threads;
    printf("Nhap m = ");
    scanf("%d", &m);
    printf("Nhap n = ");
    scanf("%d", &n);
    printf("Nhap p = ");
    scanf("%d", &p);
    int *A[m], *B[n], *C[m];
    for (i = 0; i < m; i++)
    {
        A[i] = (int *)malloc(n * sizeof(int));
        for (j = 0; j < n; j++)
            A[i][j] = rand() % 10;
        C[i] = (int *)calloc(p, sizeof(int));
    }
    for (i = 0; i < n; i++)
    {
        B[i] = (int *)malloc(p * sizeof(int));
        for (j = 0; j < p; j++)
            B[i][j] = rand() % 10;
    }
    printf("A:\n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
            printf("\t%d", A[i][j]);
        printf("\n");
    }
    printf("B:\n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < p; j++)
            printf("\t%d", B[i][j]);
        printf("\n");
    }
    num_threads = m * p;
    omp_set_num_threads(num_threads);
#pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        for(int i = 0; i < n; i++)
            

        for (int i = 0; i < p; i++)
            for (int j = 0; j < n; j++)
                C[id][i] += A[id][j] * B[j][i];
    }
    printf("A x B:\n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
            printf("\t%d", C[i][j]);
        printf("\n");
    }
    for (i = 0; i < m; i++)
    {
        free(A[i]);
        free(C[i]);
    }
    for (i = 0; i < n; i++)
        free(B[i]);
    // free(A);
    // free(B);
    // free(C);
    return 0;
}