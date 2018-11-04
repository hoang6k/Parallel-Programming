#include <stdio.h>
#include <stdlib.h>
#include <mpich/mpi.h>
#include <malloc.h>
int main(int argc, char **argv)
{
    srand(0);
    int i, n, Mc, _max, rank, size, *A, *As;
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
            A[i] = rand() % n;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    Mc = n / size;
    As = (int *)malloc(Mc * sizeof(int));
    //  Gui A cho cac CPU
    MPI_Scatter(A, Mc, MPI_INT, As, Mc, MPI_INT, 0, MPI_COMM_WORLD);
    for (i = 1; i < Mc; i++)
        As[0] = As[0] > As[i] ? As[0] : As[i];
    //  Nhan A va tinh MAX tu cac CPU
    MPI_Reduce(As, &_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        for (i = 0; i < n; i++)
            printf("%d\t", A[i]);
        printf("\nGia tri lon nhat trong %d so tren la: %d\n", n, _max);
    }
    MPI_Finalize();
    return 0;
}