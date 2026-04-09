#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cstdlib>

double* a = nullptr;
double* b = nullptr;
double* c = nullptr;
size_t m = 20000;
size_t n = 20000;

struct Edge
{
    size_t lb_row, ub_row;
    size_t lb_col, ub_col;
};
Edge* edges = nullptr;


void func_thread(Edge edge)
{
    for (size_t i = edge.lb_row; i < edge.ub_row; i++) 
    {
        size_t curr = i * n;
        for (size_t j = 0; j < n; j++)
        {
            a[curr + j] = i + j;
        }
    }
    for (size_t j = edge.lb_col; j < edge.ub_col; j++)
    {
        b[j] = j;
    }
}

void mult(Edge edge)
{
    for (size_t i = edge.lb_row; i < edge.ub_row; i++) 
    {
        double sum = 0.0;
        size_t curr = i * n;
        for (size_t j = 0; j < n; j++)
        {
            sum += a[curr + j] * b[j];
        }
        c[i] = sum;
    }
}

int main(int argc, char *argv[]) 
{

    size_t num_threads = 1;
    if (argc > 1) 
    {
        num_threads = atoi(argv[1]);
    }
    if (argc > 2) 
    {
        m = atoi(argv[2]);
    }
    if (argc > 3) 
    {
        n = atoi(argv[3]);
    }

    auto a_ptr = std::make_unique<double[]>(m * n);
    auto b_ptr = std::make_unique<double[]>(n);
    auto c_ptr = std::make_unique<double[]>(m);
    ::a = a_ptr.get();
    ::b = b_ptr.get();
    ::c = c_ptr.get();
    size_t row_per_thread = m / num_threads;
    size_t col_per_thread = n / num_threads;

    auto edges_ptr = std::make_unique<Edge[]>(num_threads);
    ::edges = edges_ptr.get();
    for (size_t thread_id = 0; thread_id < num_threads; thread_id++) 
    {
        edges[thread_id].lb_row = thread_id * row_per_thread;
        if (thread_id == num_threads - 1)
        {
            edges[thread_id].ub_row = m;
        }
        else 
        {
            edges[thread_id].ub_row = edges[thread_id].lb_row + row_per_thread;
        }

        edges[thread_id].lb_col = thread_id * col_per_thread;
        if (thread_id == num_threads - 1)
        {
            edges[thread_id].ub_col = n;
        }
        else 
        {
            edges[thread_id].ub_col = edges[thread_id].lb_col + col_per_thread;
        }
    }

    const auto start = std::chrono::steady_clock::now();

    auto threads = std::make_unique<std::jthread[]>(num_threads);
    for (size_t i = 0; i < num_threads; i++)
    {
        threads[i] = std::jthread(func_thread, edges[i]);
    }
    threads.reset();

    auto threads_mul = std::make_unique<std::jthread[]>(num_threads);
    for (size_t i = 0; i < num_threads; i++)
    {
        threads_mul[i] = std::jthread(mult, edges[i]);
    }
    threads_mul.reset();

    const auto end = std::chrono::steady_clock::now();
    
    std::cout << std::chrono::duration<double>(end - start).count() << std::endl;

    return 0;
}