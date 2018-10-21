#include <stdio.h>
#include <mpich/mpi.h>
#include <malloc.h>
#define NUM 20
void print(int *A, int *B, int *C)
{
    for (int i = 0; i < NUM; i++)
    {
        printf("A[%d]: %d", i, *(A + i));
        printf("\tB[%d]: %d", i, *(B + i));
        printf("\tC[%d]: %d\n", i, *(C + i));
    }
}
int main(int argc, char **argv)
{
    int *A, *B, *C, *As, *Bs;
    int Mc = NUM / 4;
    int rank, size;
    A = (int *)malloc(NUM * sizeof(int));
    B = (int *)malloc(NUM * sizeof(int));
    C = (int *)malloc(NUM * sizeof(int));
    As = (int *)malloc(Mc * sizeof(int));
    Bs = (int *)malloc(Mc * sizeof(int));
    for (int i = 0; i < NUM; i++)
    {
        *(A + i) = i;
        *(B + i) = 2 * i;
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    if (rank == 0)
    {
        for (int i = 1; i < 4; i++)
        {
            MPI_Send(A + i * Mc, Mc, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(B + i * Mc, Mc, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
        for (int i = 0; i < Mc; i++)
            *(C + i) = *(A + i) + *(B + i);
        for (int i = 1; i < 4; i++)
            MPI_Recv(C + i * Mc, Mc, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
        print(A, B, C);
    }
    else
    {
        MPI_Recv(As, Mc, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
        MPI_Recv(Bs, Mc, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
        for (int i = 0; i < Mc; i++)
            *(As + i) += *(Bs + i);
        MPI_Send(As, Mc, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    free(A);
    free(B);
    free(C);
    free(As);
    free(Bs);
    return 0;
}