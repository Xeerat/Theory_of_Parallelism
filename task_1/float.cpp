#include <vector>
#include <cmath>
#include <iostream>

using namespace std; 


int main() 
{
    int N = 10'000'000;
    float PI = 3.14159265358979323846;
    vector<float> data(N);

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

    cout << "Array type: float\n";
    cout << "Sum: " << sum << endl;

    return 0;
}