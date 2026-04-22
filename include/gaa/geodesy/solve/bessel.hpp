#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/geodesy/solve/solver.hpp>

namespace gaa
{
    struct Bessel_solver
    {
        Ellipsoid const &ellipsoid;

        Geodetic_solve_result solve(double latitude, double longitude, double s, double angle) const;
        Geodetic_rsolve_result rsolve(double lat1, double lon1, double lat2, double lon2, kwargs args = {}) const;
        Geodetic_rsolve_result rsolve(double lat1, double lon1, double lat2, double lon2) const
        {
            return this->rsolve(lat1, lon1, lat2, lon2, kw{});
        }

        ~Bessel_solver() = default;
        Bessel_solver(Ellipsoid const &e)
            : ellipsoid(e)
        {
        }
    };

    struct _bessel_solve_fn
    {
        double s_lat1, angle_lon1;

        Geodetic_solve_result operator()(Geodetic_coordinate const &gc) const
        {
            return Bessel_solver(gc.ellipsoid).solve(gc.latitude, gc.longitude, s_lat1, angle_lon1);
        }

        Geodetic_solve_result operator()(Geodetic_rsolve_result const &r) const
        {
            return Bessel_solver(r.ellipsoid).solve(s_lat1, angle_lon1, r.s, r.angle);
        }
    };

    GAA_weak_channel((Geodetic_solve_result), (Geodetic_coordinate), const &gc, (_bessel_solve_fn), const &fn)
    {
        return fn(gc);
    }

    GAA_weak_channel((Geodetic_solve_result), (Geodetic_rsolve_result), const &r, (_bessel_solve_fn), const &fn)
    {
        return fn(r);
    }

    struct _bessel_solve
    {
        _bessel_solve_fn operator()(double s_lat1, double angle_lon1) const
        {
            return _bessel_solve_fn{s_lat1, angle_lon1};
        }
    };

    constexpr _bessel_solve bessel = {};

    struct _bessel_rsolve_fn
    {
        double lat1_2, lon1_2;
        kwargs args;

        Geodetic_rsolve_result operator()(Geodetic_solve_result const &r) const
        {
            return Bessel_solver(r.ellipsoid).rsolve(lat1_2, lon1_2, r.latitude, r.longitude, args);
        }

        Geodetic_rsolve_result operator()(Geodetic_coordinate const &gc) const
        {
            return Bessel_solver(gc.ellipsoid).rsolve(gc.latitude, gc.longitude, lat1_2, lon1_2, args);
        }
    };

    GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_solve_result), const &r, (_bessel_rsolve_fn), const &fn)
    {
        return fn(r);
    }

    GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_coordinate), const &gc, (_bessel_rsolve_fn), const &fn)
    {
        return fn(gc);
    }

    struct _bessel_rsolve
    {
        _bessel_rsolve_fn operator()(double lat1_2, double lon1_2, kwargs args = {}) const
        {
            return _bessel_rsolve_fn{lat1_2, lon1_2, args};
        }
    };

    constexpr _bessel_rsolve rbessel = {};
}