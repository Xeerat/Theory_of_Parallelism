#include <omp.h>
#include <iostream>
#include <memory>
#include <cmath>
#include <chrono>


int main(int argc, char* argv[]) 
{

    size_t n = 5'000;

    if (argc > 1) 
    {
        n = atoi(argv[1]);
    }

    auto a = std::make_unique<double[]>(n*n);
    auto b = std::make_unique<double[]>(n);
    auto x_old = std::make_unique<double[]>(n);
    auto x_new = std::make_unique<double[]>(n);

    for (size_t i = 0; i < n; i++) 
    {
        for (size_t j = 0; j < n; j++) 
        {
            if (i==j)
            {
                a[i*n + j] = 2.0;
            }
            else
            {
                a[i*n + j] = 1.0;
            }
        }
        x_old[i] = 0.0;
        b[i] = n + 1;
    }

    double epsilon = 1e-6;
    const auto start = std::chrono::steady_clock::now();

    size_t num_iter = 1000;
    for (size_t iter = 0; iter < num_iter; iter++) 
    {
        #pragma omp parallel for
        for (size_t i = 0; i < n; i++) 
        {
            double sum = 0.0;
            size_t curr = i*n;
            for (size_t j = 0; j < n; j++) 
            {
                if (i != j)
                {
                    sum += a[curr + j] * x_old[j];
                }
            }
            x_new[i] = (b[i] - sum) / a[curr + i];    
        }
        
        double max_diff = 0.0;
        #pragma omp parallel for reduction(max:max_diff)
        for (size_t i = 0; i < n; i++) 
        {
            double curr_diff = std::fabs(x_new[i] - x_old[i]);
            if (curr_diff > max_diff)
            {
                max_diff = curr_diff;
            }
        }
        if (max_diff < epsilon) 
        {
            break;
        }

        #pragma omp parallel for
        for (size_t i = 0; i < n; i++) 
        {
            x_old[i] = x_new[i];
        }
    }
    const auto end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration<double>(end-start).count() << std::endl;
};