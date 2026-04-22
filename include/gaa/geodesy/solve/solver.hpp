#pragma once

#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa
{
    struct Geodetic_solve_result
    {
        Latitude latitude;
        Longitude longitude;
        Radian rangle;
        Ellipsoid const &ellipsoid;
    };

    struct Geodetic_rsolve_result
    {
        Radian angle;
        Radian rangle;
        double s;
        Ellipsoid const &ellipsoid;
    };
}