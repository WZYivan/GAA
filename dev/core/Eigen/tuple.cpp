#include <iostream>

#include <gaa/core/Eigen/tuple.hpp>

int main()
{
    Eigen::Vector4d vec(4);
    vec << 1, 2, 3, 4;
    auto tuple = gaa::make_tuple(vec);
    auto r_vec = gaa::make_vector(tuple);

    std::cout << r_vec.format(Eigen::IOFormat{});
    return 0;
}