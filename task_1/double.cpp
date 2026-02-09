#include <vector>
#include <cmath>
#include <iostream>

using namespace std; 


int main() 
{
    int N = 10'000'000;
    double PI = 3.14159265358979323846;
    vector<double> data(N);

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

    cout << "Array type: double\n";
    cout << "Sum: " << sum << endl;

    return 0;
}