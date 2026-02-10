#include <vector>
#include <cmath>
#include <iostream>


int main() 
{
    int N = 10'000'000;
    float PI = 3.14159265358979323846;
    std::vector<float> data(N);

    float step = 2.0f * PI / N;

    for (int i = 0; i < N; ++i) 
    {
        data[i] = sinf(step * i);
    }

    float sum = 0;
    for (const float& v : data) 
    {
        sum += v;
    }

    std::cout << "Array type: float\n";
    std::cout << "Sum: " << sum << std::endl;

    return 0;
}