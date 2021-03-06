#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <mpich/mpi.h>

#define m 100
#define n 100
#define T 20
#define dt 0.01
#define dx 0.1
#define D 0.1
//=========================
void Write2File(float *C, char *c)
{
    FILE *result = fopen("result2.txt", c);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            fprintf(result, "%lf\t", *(C + i * n + j));
        fprintf(result, "\n");
    }
    fclose(result);
}
//=========================
void KhoiTao(float *C)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            if (i >= (m / 2 - 5) && i < (m / 2 + 5) && j >= (n / 2 - 5) && j < (n / 2 + 5))
                *(C + i * n + j) = 80.0;
            else
                *(C + i * n + j) = 25.0;
        }
}
//=========================
void FD(float *C, float *dC, float *Cu, float *Cd, int Mc, int start, int end)
{
    float c, u, d, l, r;
    for (int i = start; i <= end; i++)
        for (int j = 0; j < n; j++)
        {
            c = *(C + i * n + j);
            u = (i == 0) ? Cu[j] : *(C + (i - 1) * n + j);
            d = (i == Mc - 1) ? Cd[j] : *(C + (i + 1) * n + j);
            l = (j == 0) ? 25 : *(C + i * n + j - 1);
            r = (j == n - 1) ? 25 : *(C + i * n + j + 1);
            *(dC + i * n + j) = D * (u + d + l + r - 4 * c) / (dx * dx);
        }
}
//=========================
int main(int argc, char **argv)
{
    int i, j, rank, size, Ntime = T / dt, Mc;
    float t = 0, *C, *dC, *Cs, *dCs, *Cu, *Cd;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    MPI_Request req;
    //  Khoi tao gia tri cho CPU 0
    if (rank == 0)
    {
        C = (float *)malloc(m * n * sizeof(float));
        dC = (float *)calloc(m * n, sizeof(float));
        KhoiTao(C);
        // Write2File(C, "w");
    }
    //  Khoi tao gia tri cho cac CPU
    Mc = m / size;
    Cs = (float *)malloc(Mc * n * sizeof(float));
    dCs = (float *)calloc(Mc * n, sizeof(float));
    Cu = (float *)malloc(n * sizeof(float));
    Cd = (float *)malloc(n * sizeof(float));
    //  Gui input cho cac CPU
    MPI_Scatter(C, Mc * n, MPI_FLOAT, Cs, Mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(dC, Mc * n, MPI_FLOAT, dCs, Mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //  Vong lap tinh toan Ntime lan
    time_t t1, t2;
    t1 = time(NULL);
    for (t = 0; t < Ntime; t++)
    {
        //  Truyen di mang Cu va tinh dao ham tu hang 1 den hang Mc - 2
        if (rank != size - 1)
        {
            MPI_Isend(Cs + (Mc - 1) * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &stat);
        }
        FD(Cs, dCs, Cu, Cd, Mc, 1, Mc - 2);
        //  Nhan Cu, truyen Cd va tinh dao ham cho hang 0
        if (rank != 0)
        {
            MPI_Irecv(Cu, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &stat);
            MPI_Isend(Cs, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &stat);
        }
        else
            for (i = 0; i < n; i++)
                Cu[i] = 25;
        FD(Cs, dCs, Cu, Cd, Mc, 0, 0);
        //  Nhan Cd va tinh dao ham cho hang Mc - 1
        if (rank != size - 1)
        {
            MPI_Irecv(Cd, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &stat);
        }
        else
            for (i = 0; i < n; i++)
                Cd[i] = 25;
        FD(Cs, dCs, Cu, Cd, Mc, Mc - 1, Mc - 1);
        //  Cap nhat Cs
        for (i = 0; i < Mc * n; i++)
            *(Cs + i) += dt * (*(dCs + i));
    }
    //  Nhan output tu cac CPU
    MPI_Gather(Cs, Mc * n, MPI_FLOAT, C, Mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        t2 = time(NULL);
        printf("\tThe Calculation time: %ld\n", (long)(t2 - t1));
        Write2File(C, "w");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
