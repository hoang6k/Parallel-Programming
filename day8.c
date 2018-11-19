#include <stdio.h>
#include <malloc.h>
#include <mpich/mpi.h>

int main(int argc, char **argv)
{
    int a[10000], b[10000], rank, n;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;
    n = sizeof(a) / sizeof(int);
    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            a[i] = i;
            b[i] = i + n;
        }
        MPI_Send(a, n, MPI_INT, 1, 1, MPI_COMM_WORLD);
        printf("\nCPU 0 sent a");
        MPI_Send(b, n, MPI_INT, 1, 2, MPI_COMM_WORLD);
        printf("\nCPU 0 sent b\n");
    }
    else if (rank == 1)
    {
        for (int i = 0; i < n; i++)
            printf("\na[%d] = %d\tb[%d] = %d", i, a[i], i, b[i]);
        MPI_Recv(b, n, MPI_INT, 0, 2, MPI_COMM_WORLD, &stat);
        printf("\nCPU 0 received b");
        MPI_Recv(a, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
        printf("\nCPU 0 received a\n");
        for (int i = 0; i < n; i++)
            printf("\na[%d] = %d\tb[%d] = %d", i, a[i], i, b[i]);
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}