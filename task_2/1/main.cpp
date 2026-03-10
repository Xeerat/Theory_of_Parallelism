#include <omp.h>
#include <iostream>
#include <memory>
#include <chrono>


int main(int argc, char *argv[]) 
{

    size_t n = 20'000;
    size_t m = 20'000;
    
    if (argc > 1) 
    {
        m = atoi(argv[1]);
    }
    if (argc > 2)
    {
        n = atoi(argv[2]);
    }

    auto a = std::make_unique<double[]>(m * n);
    auto b = std::make_unique<double[]>(n);
    auto c = std::make_unique<double[]>(m);

    for (size_t i = 0; i < m; i++) 
    {
        size_t curr = i * n;
        for (size_t j = 0; j < n; j++) 
        {
            a[curr + j] = i + j;
        }
    }

    for (size_t j = 0; j < n; j++)
    {
        b[j] = j;
    }


    const auto start = std::chrono::steady_clock::now();
    #pragma omp parallel
    {
        size_t nthreads = omp_get_num_threads();
        size_t threadid = omp_get_thread_num();
        size_t items_per_thread = m / nthreads;
        size_t lb = threadid * items_per_thread;
        size_t ub = 0;
        if (threadid == nthreads - 1) 
        {
            ub = (m - 1);
        }
        else
        {
            ub = (lb + items_per_thread - 1);
        }

        for (size_t i = lb; i <= ub; i++) 
        {
            c[i] = 0.0;
            for (size_t j = 0; j < n; j++) 
            {
                c[i] += a[i*n + j] * b[j];
            }
        }
    }
    const auto end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration<double>(end-start).count() << std::endl;
};