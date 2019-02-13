#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>
#include <malloc.h>

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
void FD(float *C, float *dC, float *Cu, float *Cd, int Mc)
{
    float c, u, d, l, r;
    for (int i = 0; i < Mc; i++)
    {
        for (int j = 0; j < n; j++)
        {
            c = C[i * n + j];
            u = (i == 0) ? Cu[j] : C[(i - 1) * n + j];
            d = (i == Mc - 1) ? Cd[j] : C[(i + 1) * n + j];
            l = (j == 0) ? 25 : C[i * n + j - 1];
            r = (j == n - 1) ? 25 : C[i * n + j + 1];
            *(dC + i * n + j) = D * (u + d + l + r - 4 * c) / (dx * dx);
        }
    }
}

int main(int argc, char **argv)
{
    int rank, size, Ntime = T / dt, Mc;
    float *C, *dC, *Cs, *dCs, *Cu, *Cd;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    //  Khoi tao gia tri cho CPU 0
    if (rank == 0)
    {
        C = (float *)malloc(m * n * sizeof(float));
        dC = (float *)calloc(m * n, sizeof(float));
        KhoiTao(C);
    }
    //  Khoi tao gia tri cho cac CPU
    Mc = m / size;
    Cs = (float *)malloc(Mc * n * sizeof(float));
    dCs = (float *)calloc(Mc * n, sizeof(float));
    Cu = (float *)malloc(n * sizeof(float));
    Cd = (float *)malloc(n * sizeof(float));
    //  Gui cac input cho cac CPU
    MPI_Scatter(C, Mc * n, MPI_FLOAT, Cs, Mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //  Vong lap tinh toan Ntime lan
    for (int t = 0; t < Ntime; t++)
    {
        //  Truyen thong mang Cu
        if (rank == 0)
        {
            for (int i = 0; i < n; i++)
                Cu[i] = 25.0;
            MPI_Send(Cs + (Mc - 1) * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
        }
        else if (rank == size - 1)
            MPI_Recv(Cu, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        else
        {
            MPI_Recv(Cu, n, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
            MPI_Send(Cs + (Mc - 1) * n, n, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
        }
        //  Truyen thong mang Cd
        if (rank == 0)
            MPI_Recv(Cd, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &stat);
        else if (rank == size - 1)
        {
            for (int i = 0; i < n; i++)
                Cd[i] = 25;
            MPI_Send(Cs, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(Cs, n, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
            MPI_Recv(Cd, n, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &stat);
        }
        //  Tinh toan dao ham
        FD(Cs, dCs, Cu, Cd, Mc);
        //  Cap nhat Cs
        for (int i = 0; i < Mc * n; i++)
            Cs[i] += dt * dCs[i];
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //  Nhan output tu cac CPU
    MPI_Gather(Cs, Mc * n, MPI_FLOAT, C, Mc * n, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        Write2File(C, "w");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}