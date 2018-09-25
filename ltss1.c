// #include<stdio.h>
// #include<malloc.h>
// int main() {
// 	int i, *A, *B, *C;
// 	A = (int *) malloc((10) * sizeof(int));
// 	B = (int *) malloc((10) * sizeof(int));
// 	C = (int *) malloc((10) * sizeof(int));

// 	for (i = 0; i < 10; i++) {
// 		*(A + i) = i;
// 		*(B + i) = 2 * i;
// 	}

// 	for (i = 0; i < 10; i++) {
// 		*(C + i) = *(A + i) + *(B + i);
// 	}

// 	for (i = 0; i < 10; i++) {
// 		printf("A[%d]: %d", i, *(A + i));
// 		printf("B[%d]: %d", i, *(B + i));
// 		printf("C[%d]: %d\n", i, *(C + i));
// 	}
// 	free(A);
// 	free(B);
// 	free(C);
// 	return 0;
// }

// #include<omp.h>
// #include<stdio.h>
// int main(){
// 	#pragma omp parallel
// 	printf("Hello from thread %d, nthreads %d\n",
// 		omp_get_thread_num(),
// 		omp_get_num_threads());
// 	return 0;
// }

// #include<omp.h>
// #include<stdio.h>
// int main(){
// 	int threads, id;
// 	threads = 10;
// 	id = 10;
// 	printf("Hello from thread %d, nthreads %d\n", id, threads);
// 	return 0;
// }

// #include<omp.h>
// #include<stdio.h>
// int main(){
// 	int threads, id;
// 	threads = 10;
// 	id = 10;
// 	omp_set_num_threads(threads);
// 	#pragma omp parallel
// 		// printf("Hello from thread %d, nthreads %d\n", id, threads);
// 		printf("Hello from thread %d, nthreads %d\n",
// 			omp_get_thread_num(),
// 			omp_get_num_threads());
// 	return 0;
// }

// #include<omp.h>
// #include<stdio.h>
// int main(){
// 	int id, x;
// 	omp_set_num_threads(100);
// 	#pragma omp parallel private(x)
// 	{
// 		id = omp_get_thread_num();
// 		x = 10*id;
// 		printf("\n");
// 		printf("Hello from thread %d,x = %d", id, x);
// 		printf("\n");
// 	}
// 	return 0;
// }

#include <stdio.h>
#include <malloc.h>
#include <omp.h>
#include <time.h>
int main()
{
	int i, id, *A, *B, *C;
	A = (int *)malloc((20) * sizeof(int));
	B = (int *)malloc((20) * sizeof(int));
	C = (int *)malloc((20) * sizeof(int));
	for (i = 0; i < 20; i++)
	{
		*(A + i) = i;
		*(B + i) = 2 * i;
	}
	time_t start = clock();
	omp_set_num_threads(4);
	int Ns = 20 / omp_get_num_threads();
#pragma omp parallel private(id, i)
	{
		id = omp_get_thread_num();
		// printf("id = %d, from thread %d\n", id, omp_get_thread_num());
		for (i = Ns * id; i < Ns * (id + 1); i++)
			*(C + i) = *(A + i) + *(B + i);
	}
	time_t end = clock();
	for (i = 0; i < 20; i++)
	{
		printf("A[%d]:%d", i, *(A + i));
		printf("\tB[%d]:%d", i, *(B + i));
		printf("\tC[%d]:%d\n", i, *(C + i));
	}
	printf("\nparallel: %f", (double)(end - start) / CLOCKS_PER_SEC);
	free(A);
	free(B);
	free(C);
	A = (int *)malloc((10) * sizeof(int));
	B = (int *)malloc((10) * sizeof(int));
	C = (int *)malloc((10) * sizeof(int));
	for (i = 0; i < 10; i++)
	{
		*(A + i) = i;
		*(B + i) = 2 * i;
	}
	start = clock();
	for (i = 0; i < 10; i++)
	{
		*(C + i) = *(A + i) + *(B + i);
	}
	end = clock();
	for (i = 0; i < 10; i++)
	{
		printf("A[%d]: %d", i, *(A + i));
		printf("B[%d]: %d", i, *(B + i));
		printf("C[%d]: %d\n", i, *(C + i));
	}
	printf("\nnot parallel: %f", (double)(end - start) / CLOCKS_PER_SEC);
	free(A);
	free(B);
	free(C);
	return 0;
}
