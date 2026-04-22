#include <gaa/core/time.hpp>
#include <gaa/core/keywords.hpp>

namespace gaa
{
    double julia_day(int Y, int M, int D, int H, int m, int S)
    {
        gaa_assert(Y >= 0);
        gaa_assert(M >= 0);
        gaa_assert(D >= 0);
        gaa_assert(H >= 0);
        gaa_assert(m >= 0);
        gaa_assert(S >= 0);

        if (M == 1 || M == 2)
        {
            Y = Y - 1;
            M = M + 12;
        }

        return int(365.25 * Y) + int(30.6001 * (M + 1)) + D + int(H + m / 60 + S / 3600) / 24 + 1720981.5;
    }

    double const
        julia_day_of_gps_time_origin = julia_day(1980, 1, 6, 0, 0, 0);
}