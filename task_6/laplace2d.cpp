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
        A[OFFSET(0, i, m)] = 10.0 + (20.0 - 10.0) * i / (m - 1);
        Anew[OFFSET(0, i, m)] = A[OFFSET(0, i, m)];
    }

    for (int i = 0; i < m; i++) 
    {
        A[OFFSET(n-1, i, m)] = 20.0 + (30.0 - 20.0) * i / (m - 1);
        Anew[OFFSET(n-1, i, m)] = A[OFFSET(n-1, i, m)];
    }

    for (int j = 0; j < n; j++) 
    {
        A[OFFSET(j, 0, m)] = 10.0 + (20.0 - 10.0) * j / (n - 1);
        Anew[OFFSET(j, 0, m)] = A[OFFSET(j, 0, m)];
    }

    for (int j = 0; j < n; j++) 
    {
        A[OFFSET(j, m-1, m)] = 20.0 + (30.0 - 20.0) * j / (n - 1);
        Anew[OFFSET(j, m-1, m)] = A[OFFSET(j, m-1, m)];
    }

    A[OFFSET(0, 0, m)] = 10.0;
    A[OFFSET(0, m-1, m)] = 20.0;
    A[OFFSET(n-1, 0, m)] = 20.0;
    A[OFFSET(n-1, m-1, m)] = 30.0;
}

double calcNext(double *__restrict A, double *__restrict Anew, int m, int n)
{
    double error = 0.0;
    for( int j = 1; j < n-1; j++)
    {
        for( int i = 1; i < m-1; i++ )
        {
            Anew[OFFSET(j, i, m)] = 0.25 * ( A[OFFSET(j, i+1, m)] + A[OFFSET(j, i-1, m)]
                                           + A[OFFSET(j-1, i, m)] + A[OFFSET(j+1, i, m)]);
            error = fmax( error, fabs(Anew[OFFSET(j, i, m)] - A[OFFSET(j, i , m)]));
        }
    }
    return error;
}
        
void swap(double *__restrict A, double *__restrict Anew, int m, int n)
{
    for( int j = 1; j < n-1; j++)
    {
        for( int i = 1; i < m-1; i++ )
        {
            A[OFFSET(j, i, m)] = Anew[OFFSET(j, i, m)];    
        }
    }
}

void deallocate(double *__restrict A, double *__restrict Anew)
{
    free(A);
    free(Anew);
}