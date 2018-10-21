#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define M 20
#define Time 10
#define dt 0.01
#define dx 0.1
#define D 0.1
//=========================
void DisplayArray(float *T, int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("  %.2f", *(T + i));
    printf("\n");
}
//=========================
void KhoiTao(float *T)
{
    for (int i = 0; i < M; i++)
        *(T + i) = 25.0;
}
//=========================
void Daoham(float *T, float *dT, int begin, int end)
{
    float c, l, r;
    for (int i = begin; i < end; i++)
    {
        c = *(T + i);
        l = (i == 0) ? 100.0 : *(T + (i - 1));
        r = (i == M - 1) ? 25.0 : *(T + (i + 1));
        *(dT + i) = D * (r - 2 * c + l) / (dx * dx);
    }
}
//=========================
int main()
{
    int i, Ntime, id, NT = 4, Mc = M /NT, start, end;
    float *T, *dT;
    T = (float *)malloc(M * sizeof(float));
    dT = (float *)malloc(M * sizeof(float));
    KhoiTao(T);
    printf("Gia tri khoi tao:\n");
    DisplayArray(T, M);
    Ntime = Time / dt;
    for (int t = 0; t < Ntime; t++)
    {
        omp_set_num_threads(NT);
#pragma omp parallel private(id, start, end)
        {
            id = omp_get_thread_num();
            start = id * Mc;
            end = start + Mc;
            Daoham(T, dT, start, end);
#pragma omp barrier
            for (int i = start; i < end; i++)
                *(T + i) = *(T + i) + dt * (*(dT + i));
        }
    }
    printf("Result of C:\n");
    DisplayArray(T, M);
    free(T);
    free(dT);
    return 0;
}
