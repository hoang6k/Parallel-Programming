#include <stdio.h>
#include <mpich/mpi.h>
#include <malloc.h>
int main(int argc, char **argv)
{
    int i, n, Mc, sum, rank, size, *A, *As;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status stat;
    //  Khoi tao A cho cac CPU
    if (rank == 0)
    {
        printf("Nhap vao gia tri n = ");
        scanf("%d", &n);
        A = (int *)malloc(n * sizeof(int));
        for (i = 0; i < n; i++)
            A[i] = i + 1;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    Mc = n / size;
    As = (int *)malloc(Mc * sizeof(int));
    //  Gui A cho cac CPU
    MPI_Scatter(A, Mc, MPI_INT, As, Mc, MPI_INT, 0, MPI_COMM_WORLD);
    for (i = 1; i < Mc; i++)
        As[0] += As[i];
    //  Nhan A va tinh SUM tu cac CPU
    MPI_Reduce(As, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0)
        printf("\nTong cac so tu 1 den %d la %d\n", n, sum);
    MPI_Finalize();
    return 0;
}