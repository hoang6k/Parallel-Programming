#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <mpich/mpi.h>

#define C 1
#define m 20
#define T 5
#define dt 0.01
//=========================
void DisplayMatrix(float *A, int row, int col)
{
    int i, j;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
            printf("  %f", *(A + i * col + j));
        printf("\n");
    }
}
//=========================
void Write2File(float *A, int row, int col)
{
    FILE *result = fopen("ViberStringC1.txt", "a");
    int i, j;
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            fprintf(result, "%lf\t", *(A + i * col + j));
        }
        fprintf(result, "\n");
    }
    fclose(result);
}
//=========================
void KhoiTao(float *Phi_old, float *Phi_current)
{
    int i;
    float dx, X;
    dx = 1.0 / (m - 1);
    for (i = 0; i < m; i++)
    {
        X = i * dx;
        *(Phi_old + i) = sin(2.0 * M_PI * X);
        *(Phi_current + i) = *(Phi_old + i);
    }
}
//=========================
void FD(float *sub_Phi_current, float *sub_dPhi, float left_Phi_current, float right_Phi_current, int Mc)
{
    float c, l, r;
    for (int i = 0; i < Mc; i++)
    {
        c = *(sub_Phi_current + i);
        l = (i == 0) ? left_Phi_current : *(sub_Phi_current + i - 1);
        r = (i == Mc - 1) ? right_Phi_current : *(sub_Phi_current + i + 1);
        *(sub_dPhi + i) = l - 2 * c + r;
    }
}
//=========================
int main(int argc, char **argv)
{
    int i, t, rank, size, Mc, Ntime = T / dt;
    float dx, tau, *Phi_old, *Phi_current, *Phi_new, *dPhi;
    float *sub_Phi_old, *sub_Phi_current, *sub_Phi_new, *sub_dPhi, left_Phi_current, right_Phi_current;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    //  Khoi tao gia tri cho CPU 0
    if (rank == 0)
    {
        Phi_old = (float *)malloc(m * sizeof(float));
        Phi_current = (float *)malloc(m * sizeof(float));
        Phi_new = (float *)malloc(m * sizeof(float));
        dPhi = (float *)malloc(m * sizeof(float));
        KhoiTao(Phi_old, Phi_current);
        // Write2File(Phi_current, m, 1);
    }
    //  Khoi tao gia tri cho cac CPU
    dx = 1.0 / (m - 1);
    tau = C * dt / dx;
    Mc = m / size;
    sub_Phi_old = (float *)malloc(Mc * sizeof(float));
    sub_Phi_current = (float *)malloc(Mc * sizeof(float));
    sub_Phi_new = (float *)malloc(Mc * sizeof(float));
    sub_dPhi = (float *)malloc(Mc * sizeof(float));
    //  Gui input cho cac CPU
    MPI_Scatter(Phi_old, Mc, MPI_FLOAT, sub_Phi_old, Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatter(Phi_current, Mc, MPI_FLOAT, sub_Phi_current, Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //  Vong lap tinh toan Ntime lan
    for (t = 0; t < Ntime; t++)
    {
        //  Truyen thong phan tu left_Phi_current
        if (rank == 0)
        {
            left_Phi_current = -sub_Phi_current[1];
            MPI_Send(sub_Phi_current + Mc - 1, 1, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
        }
        else if (rank == size - 1)
            MPI_Recv(&left_Phi_current, 1, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        else
        {
            MPI_Send(sub_Phi_current + Mc - 1, 1, MPI_FLOAT, rank + 1, rank, MPI_COMM_WORLD);
            MPI_Recv(&left_Phi_current, 1, MPI_FLOAT, rank - 1, rank - 1, MPI_COMM_WORLD, &stat);
        }
        //  Truyen thong phan tu right_Phi_current
        if (rank == 0)
            MPI_Recv(&right_Phi_current, 1, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &stat);
        else if (rank == size - 1)
        {
            right_Phi_current = -sub_Phi_current[Mc - 2];
            MPI_Send(sub_Phi_current, 1, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(sub_Phi_current, 1, MPI_FLOAT, rank - 1, rank, MPI_COMM_WORLD);
            MPI_Recv(&right_Phi_current, 1, MPI_FLOAT, rank + 1, rank + 1, MPI_COMM_WORLD, &stat);
        }
        //  Tinh toan dao ham
        FD(sub_Phi_current, sub_dPhi, left_Phi_current, right_Phi_current, Mc);
        //  Cap nhat sub_Phi_new
        for (i = 0; i < Mc; i++)
            *(sub_Phi_new + i) = 2 * (*(sub_Phi_current + i)) - *(sub_Phi_old + i) + tau * tau * (*(sub_dPhi + i));
        //  Cap nhat sub_Phi_old va sub_Phi_current
        for (i = 0; i < Mc; i++)
        {
            *(sub_Phi_old + i) = *(sub_Phi_current + i);
            *(sub_Phi_current + i) = *(sub_Phi_new + i);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    //  Gui va nhan output tu cac CPU
    MPI_Gather(sub_Phi_current, Mc, MPI_FLOAT, Phi_current, Mc, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Gia tri cuoi cung:\n");
        DisplayMatrix(Phi_current, m, 1);
    }
    MPI_Finalize();
    return 0;
}
