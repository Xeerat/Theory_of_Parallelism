#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <fstream>
#include <chrono>
#include <cmath>


double func(double x) 
{
    return exp(-x*x);
};

int main(int argc, char* argv[]) 
{

    size_t nstep = 40'000'000;

    if (argc > 1) 
    {
        nstep = atoi(argv[1]);
    }

    double a = -4.0;
    double b = 4.0;
    double h = (b - a) / nstep;
    double sum = 0.0;
  
    auto start = std::chrono::steady_clock::now();
    #pragma omp parallel reduction(+:sum)
    {
        size_t nthreads = omp_get_num_threads();
        size_t threadid = omp_get_thread_num();
        size_t per_thread = nstep / nthreads;
        size_t lb = threadid * per_thread;
        size_t ub = 0;
        if (threadid == nthreads - 1)
        {
            ub = nstep;
        }
        else
        {
            ub = lb + per_thread;
        }

        double x = a + h * (lb + 0.5);
        for (size_t i = lb; i < ub; i++)
        {   
            sum += func(x);
            x += h;
        }
    }
    sum *= h;

    auto end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
};