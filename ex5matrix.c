#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>
#include <malloc.h>
void print(int *A, int *B, int *C, int Mc, int m, int n, int p);
int main(int argc, char **argv)
{
    srand(0);
    int i, j, rank, size, Mc, m, n, p, *A, *B, *C;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    //  Khoi tao gia tri dau vao cho CPU 0
    if (rank == 0)
    {
        printf("Nhap m = ");
        scanf("%d", &m);
        printf("Nhap n = ");
        scanf("%d", &n);
        printf("Nhap p = ");
        scanf("%d", &p);
        A = (int *)malloc(m * n * sizeof(int));
        B = (int *)malloc(n * p * sizeof(int));
        C = (int *)calloc(m * p, sizeof(int));
        for (i = 0; i < m * n; i++)
            A[i] = rand() % 10;
        for (i = 0; i < n * p; i++)
            B[i] = rand() % 10;
    }
    //  Khoi tao gia tri cho cac CPU
    MPI_Bcast(&m, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&p, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    Mc = m / size;
    if (rank != 0)
    {
        A = (int *)malloc(Mc * n * sizeof(int));
        B = (int *)malloc(n * p * sizeof(int));
        C = (int *)malloc(Mc * p * sizeof(int));
    }
    //  Gui B cho cac CPU
    MPI_Bcast(B, n * p, MPI_INT, 0, MPI_COMM_WORLD);
    //  Gui va nhan A cho cac CPU
    if (rank == 0)
        for (i = 1; i < size; i++)
            MPI_Send(A + i * Mc * n, Mc * n, MPI_INT, i, 0, MPI_COMM_WORLD);
    else
        MPI_Recv(A, Mc * n, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
    //  Tinh toan
    for (i = 0; i < Mc * p; i++)
        for (j = 0; j < n; j++)
            *(C + i) += *(A + n * (i / p) + j) * *(B + j * p + (i % p));
    //  Gui va nhan output tu cac CPU
    if (rank == 0)
    {
        for (i = 1; i < size; i++)
            MPI_Recv(C + i * Mc * p, Mc * p, MPI_INT, i, i, MPI_COMM_WORLD, &stat);
        print(A, B, C, Mc, m, n, p);
    }
    else
        MPI_Send(C, Mc * p, MPI_INT, 0, rank, MPI_COMM_WORLD);
    free(A);
    free(B);
    free(C);
    MPI_Finalize();
    return 0;
}
void print(int *A, int *B, int *C, int Mc, int m, int n, int p)
{
    printf("A:\n");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            printf("\t%d", *(A + i * n + j));
        printf("\n");
    }
    printf("B:\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < p; j++)
            printf("\t%d", *(B + i * p + j));
        printf("\n");
    }
    printf("A x B voi 1 thread la %d hang:\n", Mc);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
            printf("\t%d", *(C + i * p + j));
        printf("\n");
    }
}