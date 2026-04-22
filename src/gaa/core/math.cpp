#include <numbers>

#include <gaa/core/keywords.hpp>
#include <gaa/core/math.hpp>

namespace gaa
{
    bool within_abs(double x, double y, double epsilon)
    {
        return std::abs(x - y) < epsilon;
    }

    double round(double x)
    {
        double int_part;
        double frac_part = std::modf(x, &int_part);

        if (within_abs(frac_part, 0.5))
        {
            if (within_abs(std::fmod(int_part, 2), 0))
            {
                return int_part;
            }
            else
            {
                return int_part + (x >= 0 ? 1 : -1);
            }
        }
        else if (std::abs(frac_part) < 0.5)
        {
            return int_part;
        }
        else // (std::abs(frac_part) > 0.5)
        {
            return int_part + (x >= 0 ? 1 : -1);
        }
    }

    double cut(double x, int precision)
    {
        if (precision < 0)
        {
            return x;
        }

        if (precision == 0)
        {
            return round(x);
        }
        double scale = std::pow(10, precision);
        return round(x * scale) / scale;
    }

    double deg2rad(double d)
    {
        return d / 180.0 * std::numbers::pi;
    }

    double min2rad(double m)
    {
        return m / (180.0 * 60.0) * std::numbers::pi;
    }

    double sec2rad(double s)
    {
        return s / (180.0 * 3600.0) * std::numbers::pi;
    }

    double rad2deg(double r)
    {
        return r / std::numbers::pi * 180.0;
    }

    double rad2min(double r)
    {
        return rad2deg(r) * 60;
    }

    double rad2sec(double r)
    {
        return rad2deg(r) * 3600;
    }

    double dms(double d, double m, double s)
    {
        gaa_assert(m >= 0 && s >= 0, "only degree with sign is acceptable");
        bool neg = std::signbit(d);
        d = std::abs(d);
        double r = deg2rad(d) + min2rad(m) + sec2rad(s);
        return neg ? -r : r;
    }

    double const
        rho0 = 57.295'779'513'082'321'0,
        rho1 = 3'437.746'770'784'939'17,
        rho2 = 206'264.806'247'096'355;
}