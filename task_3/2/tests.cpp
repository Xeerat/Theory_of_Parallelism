#include <fstream>
#include <iostream>
#include <cmath>

double my_sin(double x)
{
    return std::sin(x);
}

double my_sqrt(double x)
{
    return std::sqrt(x);
}

double my_pow(double x, double y)
{
    return std::pow(x, y);
}

bool compare(const std::string& ref, const std::string& res)
{
    std::ifstream f1(ref), f2(res);

    double a, b;
    size_t i = 0;

    while (f1 >> a && f2 >> b)
    {
        if (std::abs(a - b) > 1e-6)
        {
            std::cout << "Mismatch in " << res
                      << " at " << i
                      << " expected=" << a
                      << " got=" << b << "\n";
            return false;
        }
        i++;
    }

    std::cout << res << " OK\n";
    return true;
}

int main()
{
    std::ifstream data("data.txt");

    std::ofstream ref_sin("ref_sin.txt");
    std::ofstream ref_sqrt("ref_sqrt.txt");
    std::ofstream ref_pow("ref_pow.txt");

    double x, y;

    while (data >> x >> y)
    {
        ref_sin  << my_sin(x) << "\n";
        ref_sqrt << my_sqrt(x) << "\n";
        ref_pow  << my_pow(x, y) << "\n";
    }

    
    ref_sin.close();
    ref_sqrt.close();
    ref_pow.close();

    compare("ref_sin.txt",  "res_sin.txt");
    compare("ref_sqrt.txt", "res_sqrt.txt");
    compare("ref_pow.txt",  "res_pow.txt");

    return 0;
}