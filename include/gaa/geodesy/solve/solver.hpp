#pragma once

#include <gaa/geodesy/ellipsoids.hpp>

namespace gaa
{
    struct Geodetic_solve_result
    {
        Latitude latitude;
        Longitude longitude;
        Azimuth rangle;
        Ellipsoid const &ellipsoid;
    };

    struct Geodetic_rsolve_result
    {
        Azimuth angle;
        Azimuth rangle;
        double s;
        Ellipsoid const &ellipsoid;
    };
}