#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <malloc.h>
#include <time.h>
int main()
{
    srand(0);
    int i, j, k, NT, m, n, p, id, num_threads;
    printf("Nhap NT = ");
    scanf("%d", &NT);
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
        int row = id / p, col = id % p;
        for (int i = 0; i < n; i++)
            C[row][col] += A[row][i] * B[i][col];
    }
    printf("A x B voi 1 thread/1 gia tri:\n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
            printf("\t%d", C[i][j]);
        printf("\n");
    }

    for (i = 0; i < m; i++)
        memset(C[i], 0, p * sizeof(int));
    num_threads = p;
    omp_set_num_threads(num_threads);
#pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                C[i][id] += A[i][j] * B[j][id];
    }
    printf("A x B voi 1 thread/1 cot:\n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
            printf("\t%d", C[i][j]);
        printf("\n");
    }

    for (i = 0; i < m; i++)
        memset(C[i], 0, p * sizeof(int));
    num_threads = m;
    omp_set_num_threads(num_threads);
#pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        for (int i = 0; i < p; i++)
            for (int j = 0; j < n; j++)
                C[id][i] += A[id][j] * B[j][i];
    }
    printf("A x B voi 1 thread/1 hang:\n");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
            printf("\t%d", C[i][j]);
        printf("\n");
    }

    int start, end;
    int Mc = m / NT;
    for (i = 0; i < m; i++)
        memset(C[i], 0, p * sizeof(int));
    num_threads = NT;
    omp_set_num_threads(num_threads);
#pragma omp parallel private(id, start, end)
    {
        id = omp_get_thread_num();
        start = id * Mc;
        end = start + Mc;
        for (int k = start; k < end; k++)
            for (int i = 0; i < p; i++)
                for (int j = 0; j < n; j++)
                    C[k][i] += A[k][j] * B[j][i];
    }
    printf("A x B voi 1 thread/%d hang:\n", NT);
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
    return 0;
}