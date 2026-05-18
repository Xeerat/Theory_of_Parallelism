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

double calcNext(double *__restrict A, double *__restrict Anew, int m, int n)
{
    double error = 0.0;
    #pragma acc parallel loop reduction(max:error) present(A,Anew)
    for( int j = 1; j < n-1; j++)
    {
        #pragma acc loop
        for( int i = 1; i < m-1; i++ )
        {
            Anew[OFFSET(j, i, m)] = 0.25 * ( A[OFFSET(j, i+1, m)] + A[OFFSET(j, i-1, m)]
                                           + A[OFFSET(j-1, i, m)] + A[OFFSET(j+1, i, m)]);
            error = fmax( error, fabs(Anew[OFFSET(j, i, m)] - A[OFFSET(j, i , m)]));
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