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
  
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (int i = 0; i < nstep; i++) 
    {
        sum += func(a + h * (i + 0.5));
    }
    sum *= h;

    auto end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
};