
#include <util/util.hpp>

float Util_RandomSingle()
{
    std::mt19937 rng(std::random_device{}());

    return std::generate_canonical<float, 128>(rng);
}

double Util_RandomDouble()
{
    std::mt19937 rng(std::random_device{}());

    return std::generate_canonical<double, 128>(rng);
}