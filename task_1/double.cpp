#include <vector>
#include <cmath>
#include <iostream>


int main() 
{
    int N = 10'000'000;
    double PI = 3.14159265358979323846;
    std::vector<double> data(N);

    double step = 2 * PI / N;

    for (int i = 0; i < N; ++i) 
    {
        data[i] = sin(step * i);
    }

    double sum = 0;
    for (const double& v : data) 
    {
        sum += v;
    }

    std::cout << "Array type: double\n";
    std::cout << "Sum: " << sum << std::endl;

    return 0;
}