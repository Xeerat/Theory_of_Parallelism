#include <cmath>
#include <cstdlib>
#include <cstring>

#define OFFSET(x, y, m) (((x)*(m)) + (y))

void initialize(double *__restrict A, double *__restrict Anew, int m, int n)
{
    memset(A, 0, n * m * sizeof(double));
    memset(Anew, 0, n * m * sizeof(double));

    for (int i = 0; i < m; i++) 
    {
        double v = 10.0 + (20.0 - 10.0) * i / (m - 1);
        A[i] = v;
        Anew[i] = v;
    }

    for (int i = 0; i < m; i++) 
    {
        double v = 20.0 + (30.0 - 20.0) * i / (m - 1);
        A[(n - 1) * m + i] = v;
        Anew[(n - 1) * m + i] = v;
    }

    for (int j = 0; j < n; j++) 
    {
        double vL = 10.0 + (20.0 - 10.0) * j / (n - 1);
        double vR = 20.0 + (30.0 - 20.0) * j / (n - 1);

        A[j * m] = vL;
        Anew[j * m] = vL;

        A[j * m + (m - 1)] = vR;
        Anew[j * m + (m - 1)] = vR;
    }

    A[0] = 10.0;
    A[m - 1] = 20.0;
    A[(n - 1) * m] = 20.0;
    A[n * m - 1] = 30.0;

    Anew[0] = 10.0;
    Anew[m - 1] = 20.0;
    Anew[(n - 1) * m] = 20.0;
    Anew[n * m - 1] = 30.0;
    
    #pragma acc enter data copyin(A[:m*n],Anew[:m*n])
}

double calcNext(double *A, double *Anew, int m, int n)
{
    double error = 0.0;

    #pragma acc parallel loop reduction(max:error) present(A,Anew)
    for (int j = 1; j < n - 1; j++)
    {
        double *row     = &A[j * m];
        double *row_up  = &A[(j - 1) * m];
        double *row_dn  = &A[(j + 1) * m];

        #pragma acc loop
        for (int i = 1; i < m - 1; i++)
        {
            double val =
                0.25 * (
                    row[i + 1] +
                    row[i - 1] +
                    row_up[i] +
                    row_dn[i]
                );

            Anew[j * m + i] = val;

            double diff = val - row[i];
            if (diff < 0) diff = -diff;
            if (diff > error) error = diff;
        }
    }

    return error;
}
        

void deallocate(double *__restrict A, double *__restrict Anew)
{
    #pragma acc exit data delete(A,Anew)
    free(A);
    free(Anew);
}