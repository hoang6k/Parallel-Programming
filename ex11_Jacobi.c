#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define m 100
#define n 100
#define epsilon 0.00000001
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
void FD(float *C_old, float *C_new)
{
  int i, j;
  float c, u, d, l, r;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
    {
      c = *(C_old + i * n + j);
      u = (i == 0) ? 25 : *(C_old + (i - 1) * n + j);
      d = (i == m - 1) ? 25 : *(C_old + (i + 1) * n + j);
      l = (j == 0) ? 25 : *(C_old + i * n + j - 1);
      r = (j == n - 1) ? 25 : *(C_old + i * n + j + 1);
      C_new[i * n + j] = 1.0 / 4 * (u + d + l + r - 4 * c);
    }
}
//=========================
int main()
{
  int i, j;
  float _max = epsilon, temp;
  float *C_old, *C_new;
  C_old = (float *)malloc((m * n) * sizeof(float));
  C_new = (float *)malloc((m * n) * sizeof(float));
  KhoiTao(C_new);
  while (_max >= epsilon)
  {
    _max = 0;
    C_old = C_new;
    FD(C_old, C_new);
    for (i = 0; i < m; i++)
      for (j = 0; j < n; j++){
        temp = C_old[i * n + j] - C_new[i * n + j];
        if (temp >= 0)
            _max = _max > temp ? _max : temp;
        else
            _max = _max > - temp ? _max : - temp;
      }
  }
  Write2File(C_new);
  return 0;
}
