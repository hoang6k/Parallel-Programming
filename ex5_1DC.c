#include <stdio.h>
#include <mpich/mpi.h>
#include <malloc.h>
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
void Daoham(float *T, float *dT, float Tl, float Tr, int Mc)
{
    float c, l, r;
    for (int i = 0; i < Mc; i++)
    {
        c = *(T + i);
        l = (i == 0) ? Tl : *(T + (i - 1));
        r = (i == Mc - 1) ? Tr : *(T + (i + 1));
        *(dT + i) = (r - 2 * c + l) / (dx * dx);
    }
}
//=========================
int main(int argc, char **argv)
{
    int i, t, Ntime = Time / dt, rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    //  Khoi tao gia tri cho cac CPU
    int Mc = M / size;
    float *T, *dT, Tl, Tr;
    if (rank == 0)
    {
        dT = (float *)malloc(M * sizeof(float));
        T = (float *)malloc(M * sizeof(float));
        KhoiTao(T);
        printf("Gia tri khoi tao:\n");
        DisplayArray(T, M);
    }
    else
    {
        dT = (float *)malloc(Mc * sizeof(float));
        T = (float *)malloc(Mc * sizeof(float));
    }
    //  Gui va nhan input cho cac CPU
    if (rank == 0)
        for (i = 1; i < size; i++)
        {
            MPI_Send(T + i * Mc, Mc, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            MPI_Send(dT + i * Mc, Mc, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
        }
    else
    {
        MPI_Recv(T, Mc, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &stat);
        MPI_Recv(dT, Mc, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &stat);
    }
    //  Vong lap tinh toan Ntime lan
    for (t = 0; t < Ntime; t++)
    {
        //  Truyen thong phan tu Tl
        if (rank == 0)
        {
            Tl = 100;
            MPI_Send(T + Mc - 1, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
        }
        else if (rank == size - 1)
            MPI_Recv(&Tl, 1, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        else
        {
            MPI_Send(T + Mc - 1, 1, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
            MPI_Recv(&Tl, 1, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        }
        //  Truyen thong phan tu Tr
        if (rank == 0)
            MPI_Recv(&Tr, 1, MPI_FLOAT, 1, 1, MPI_COMM_WORLD, &stat);
        else if (rank == size - 1)
        {
            Tr = 25;
            MPI_Send(T, 1, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(T, 1, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
            MPI_Recv(&Tr, 1, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &stat);
        }
        //  Tinh toan dao ham
        Daoham(T, dT, Tl, Tr, Mc);
        //  Cap nhat T
        for (i = 0; i < Mc; i++)
            *(T + i) = *(T + i) + D * dt * (*(dT + i));
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //  Gui va nhan output tu cac CPU
    if (rank == 0)
    {
        for (i = 1; i < size; i++)
            MPI_Recv(T + i * Mc, Mc, MPI_FLOAT, i, i, MPI_COMM_WORLD, &stat);
        printf("Result of C:\n");
        DisplayArray(T, M);
    }
    else
        MPI_Send(T, Mc, MPI_FLOAT, 0, rank, MPI_COMM_WORLD);
    MPI_Finalize();
    free(dT);
    free(T);
    return 0;
}
