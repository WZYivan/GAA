#include <print>

#include <gaa/core/math.hpp>
#include <gaa/geodesy/solve/bessel.hpp>

int main()
{
    {
        gaa::Latitude B1(gaa::dms(47, 46, 52.647'0));
        gaa::Longitude L1(gaa::dms(35, 49, 36.330'0));
        gaa::Radian A1(gaa::dms(44, 12, 13.664));
        double S = 44'797.282'6;

        auto ri =
            gaa::cgcs2000.coordinate(B1, L1) >>
            gaa::bessel(S, A1) >>
            gaa::rbessel(B1, L1,
                         gaa::kw{}.converge_threshold(1e-10));

        double
            da = gaa::rad2sec((A1 - ri.angle).value()),
            ds = S - ri.s;
        gaa_assert(gaa::within_abs(da, 0, 1e-2));
        gaa_assert(gaa::within_abs(ds, 0, 1e-2));
    }

    {
        gaa::Latitude
            B1(gaa::dms(47, 46, 52.647'0)),
            B2(gaa::dms(48, 4, 9.638'4));
        gaa::Longitude
            L1(gaa::dms(35, 49, 36.330'0)),
            L2(gaa::dms(36, 14, 45.050'5));

        auto ri = gaa::cgcs2000.coordinate(B1, L1) >> gaa::rbessel(B2, L2);
        auto rf = ri >> gaa::bessel(B1, L1);

        double
            dl = gaa::rad2sec((rf.longitude - L2).value()),
            db = gaa::rad2sec((rf.latitude - B2).value()),
            da = gaa::rad2sec((rf.rangle - ri.rangle).value());
        gaa_assert(gaa::within_abs(dl, 0, 1e-2));
        gaa_assert(gaa::within_abs(db, 0, 1e-3));
        gaa_assert(gaa::within_abs(da, 0, 1));
    }
}