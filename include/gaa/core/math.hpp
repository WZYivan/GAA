#pragma once

#include <cmath>

namespace gaa
{
    extern bool within_abs(double x, double y, double epsilon = 1e-10);
    extern double round(double x);
    extern double cut(double x, int precision);
    extern double deg2rad(double);
    extern double min2rad(double);
    extern double sec2rad(double);
    extern double rad2deg(double);
    extern double rad2min(double);
    extern double rad2sec(double);
    extern double dms(double, double, double = 0);

    extern double const rho0, rho1, rho2;
}