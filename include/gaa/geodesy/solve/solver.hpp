#pragma once

#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa
{
    struct Geodetic_solve_result
    {
        double latitude, longitude, rangle;
        Ellipsoid const &ellipsoid;
    };

    struct Geodetic_rsolve_result
    {
        double angle, rangle, s;
        Ellipsoid const &ellipsoid;
    };

    template <class T>
    concept Geodetic_Solver = requires(T t, double d) {
        { t.solve(d, d, d, d) } -> std::same_as<Geodetic_solve_result>;
        { t.rsolve(d, d, d, d) } -> std::same_as<Geodetic_rsolve_result>;
    };
}