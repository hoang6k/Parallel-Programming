#include <stdio.h>
#include <malloc.h>
#include <cuda.h>

#define M 20

__global__ void add(int *A, int *B, int *C) {
	int i = threadIdx.x;
	C[i] = A[i] + B[i];
}
int main() {
	int i, *A, *B, *C;
	A = (int *) malloc(M * sizeof(int));
	B = (int *) malloc(M * sizeof(int));
	C = (int *) malloc(M * sizeof(int));
	for (i = 0; i < M; i++) {
		*(A + i) = i;
		*(B + i) = 2 * i;
	}
	int *dev_A, *dev_B, *dev_C;
	int size = sizeof(int);
	// allocate device copies of a, b, c
	cudaMalloc((void**) &dev_A, size * M);
	cudaMalloc((void**) &dev_B, size * M);
	cudaMalloc((void**) &dev_C, size * M);
	cudaMemcpy(dev_A, A, size * M, cudaMemcpyHostToDevice);
	cudaMemcpy(dev_B, B, size * M, cudaMemcpyHostToDevice);
	// launch add() kernel on GPU, passing parameters
	add<<<1, M>>>(dev_A, dev_B, dev_C);
	// copy device result back to host copy of c
	cudaMemcpy(C, dev_C, size * M, cudaMemcpyDeviceToHost);
	for (i = 0; i < M; i++)
		printf("%d\n", C[i]);
	free(A);
	free(B);
	free(C);
	cudaFree(dev_A);
	cudaFree(dev_B);
	cudaFree(dev_C);
	return 0;
}
