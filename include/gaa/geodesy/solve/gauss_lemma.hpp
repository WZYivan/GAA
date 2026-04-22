#pragma once

#include <gaa/core/kw.hpp>
#include <gaa/geodesy/solve/solver.hpp>

namespace gaa
{
    struct Gauss_lemma_solver
    {
        Ellipsoid const &ellipsoid;

        Geodetic_solve_result solve(double latitude, double longitude, double s, double angle, kwargs args = {}) const;
        Geodetic_solve_result solve(double lat1, double lon1, double s, double angle) const
        {
            return this->solve(lat1, lon1, s, angle, kw{});
        }
        Geodetic_rsolve_result rsolve(double lat1, double lon1, double lat2, double lon2) const;

        ~Gauss_lemma_solver() = default;
        Gauss_lemma_solver(Ellipsoid const &e)
            : ellipsoid(e)
        {
        }
    };

    struct _gauss_lemma_solver_solve_fn
    {
        double s_lat1, angle_lon1;
        kwargs args;

        Geodetic_solve_result operator()(Geodetic_coordinate const &gc) const
        {
            return Gauss_lemma_solver(gc.ellipsoid).solve(gc.latitude, gc.longitude, s_lat1, angle_lon1, args);
        }

        Geodetic_solve_result operator()(Geodetic_rsolve_result const &r) const
        {
            return Gauss_lemma_solver(r.ellipsoid).solve(s_lat1, angle_lon1, r.s, r.angle, args);
        }
    };

    GAA_weak_channel((Geodetic_solve_result), (Geodetic_coordinate), const &gc, (_gauss_lemma_solver_solve_fn), const &fn)
    {
        return fn(gc);
    }

    GAA_weak_channel((Geodetic_solve_result), (Geodetic_rsolve_result), const &r, (_gauss_lemma_solver_solve_fn), const &fn)
    {
        return fn(r);
    }

    struct _gauss_lemma_solve
    {
        _gauss_lemma_solver_solve_fn operator()(double s_lat1, double angle_lon1, kwargs args = {}) const
        {
            return _gauss_lemma_solver_solve_fn{s_lat1, angle_lon1, args};
        }
    };

    constexpr _gauss_lemma_solve gauss_lemma = {};

    struct _gauss_lemma_rsolve_fn
    {
        double lat1_2, lon1_2;

        Geodetic_rsolve_result operator()(Geodetic_solve_result const &r) const
        {
            return Gauss_lemma_solver(r.ellipsoid).rsolve(lat1_2, lon1_2, r.latitude, r.longitude);
        }

        Geodetic_rsolve_result operator()(Geodetic_coordinate const &gc) const
        {
            return Gauss_lemma_solver(gc.ellipsoid).rsolve(gc.latitude, gc.longitude, lat1_2, lon1_2);
        }
    };

    GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_solve_result), const &r, (_gauss_lemma_rsolve_fn), const &fn)
    {
        return fn(r);
    }

    GAA_weak_channel((Geodetic_rsolve_result), (Geodetic_coordinate), const &gc,
                     (_gauss_lemma_rsolve_fn), const &fn)
    {
        return fn(gc);
    }

    struct _gauss_lemma_rsolve
    {
        _gauss_lemma_rsolve_fn operator()(double lat1_2, double lon1_2) const
        {
            return _gauss_lemma_rsolve_fn{lat1_2, lon1_2};
        }
    };

    constexpr _gauss_lemma_rsolve rgauss_lemma = {};
}