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
    int i, rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    int Mc = NUM / size, *A, *B, *C, *As, *Bs, *Cs;
    As = (int *)malloc(Mc * sizeof(int));
    Bs = (int *)malloc(Mc * sizeof(int));
    Cs = (int *)calloc(Mc, sizeof(int));
    if (rank == 0)
    {
        A = (int *)malloc(NUM * sizeof(int));
        B = (int *)malloc(NUM * sizeof(int));
        C = (int *)calloc(NUM, sizeof(int));
        for (i = 0; i < NUM; i++)
        {
            *(A + i) = i;
            *(B + i) = 2 * i;
        }
    }
    MPI_Scatter(A, Mc, MPI_INT, As, Mc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, Mc, MPI_INT, Bs, Mc, MPI_INT, 0, MPI_COMM_WORLD);
    for (i = 0; i < Mc; i++)
        *(Cs + i) = *(As + i) + *(Bs + i);
    MPI_Gather(Cs, Mc, MPI_INT, C, Mc, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
        for (i = 0; i < NUM; i++)
            printf("%d\t", *(C + i));
    MPI_Finalize();
    return 0;
}