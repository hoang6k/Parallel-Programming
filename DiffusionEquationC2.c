#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define m 100
#define n 100
#define T 20
#define dt 0.01
#define dx 0.1
#define D 0.1
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
void Write2File(float *C)
{
  FILE *result = fopen("result2.txt", "w");
  int i, j;

  for (i = 0; i < m; i++)
  {
    for (j = 0; j < n; j++)
    {
      fprintf(result, "%lf\t", *(C + i * n + j));
    }
    fprintf(result, "\n");
  }

  fclose(result);
}
//=========================
void KhoiTao(float *C)
{
  int i, j;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
    {
      if (i >= (m / 2 - 5) && i < (m / 2 + 5) && j >= (n / 2 - 5) && j < (n / 2 + 5))
        *(C + i * n + j) = 80.0;
      else
        *(C + i * n + j) = 25.0;
    }
}
//=========================
void FD(float *C, float *dC)
{
  int i, j;
  float c, u, d, l, r;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
    {
      c = *(C + i * n + j);
      u = (i == 0) ? 25 : *(C + (i - 1) * n + j);
      d = (i == m - 1) ? 25 : *(C + (i + 1) * n + j);
      l = (j == 0) ? 25 : *(C + i * n + j - 1);
      r = (j == n - 1) ? 25 : *(C + i * n + j + 1);
      *(dC + i * n + j) = (D / (dx * dx)) * (u + d + l + r - 4 * c);
    }
}
//=========================
int main()
{
  int i, j, count;
  float t;
  time_t t1, t2;
  t = 0;
  float *C, *dC;
  C = (float *)malloc((m * n) * sizeof(float));
  dC = (float *)malloc((m * n) * sizeof(float));
  KhoiTao(C);
  // Write2File(C);
  //printf("Gia tri khoi tao:\n");
  //DisplayMatrix(C, m, n);
  count = 0;
  t1 = time(NULL);
  while (t <= T)
  {
    FD(C, dC);
    for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
        *(C + i * n + j) = *(C + i * n + j) + dt * (*(dC + i * n + j));
    t = t + dt;
    count = count + 1;
    // if (count%5==0) Write2File(C);
  }
  Write2File(C);
  t2 = time(NULL);
  //FD(C, dC);
  //printf("Gia tri cuoi cung:\n");
  //DisplayMatrix(C, m, n);
  //
  printf("\tThe Calculation time:%ld\n", (long)(t2 - t1));
  return 0;
}
