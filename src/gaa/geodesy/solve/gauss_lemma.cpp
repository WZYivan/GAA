#include <cmath>
#include <gaa/core/math.hpp>
#include <gaa/geodesy/solve/gauss_lemma.hpp>

namespace gaa
{
    struct _gauss_lemma_coeff_t
    {
        double r01, r21, r03, S10, S12, S30, t01, t21, t03;

        double u(double dLs, double dBs) const noexcept
        {
            return r01 * dLs + r21 * std::pow(dBs, 2) * dLs + r03 * std::pow(dLs, 3);
        }

        double v(double dLs, double dBs) const noexcept
        {
            return S10 * dBs + S12 * dBs * std::pow(dLs, 2) + S30 * std::pow(dBs, 3);
        }

        double deltaAs(double dLs, double dBs) const noexcept
        {
            return t01 * dLs + t21 * std::pow(dBs, 2) * dLs + t03 * std::pow(dLs, 3);
        }

        ~_gauss_lemma_coeff_t() = default;
        _gauss_lemma_coeff_t(double Bm, Ellipsoid const &ellipsoid)
        {
            auto lat_const = ellipsoid.lat_aux(Latitude(Bm));
            double
                V = lat_const.v,
                t = lat_const.t,
                n2 = lat_const.nu_2,
                cosB = std::cos(Bm),
                ps = rho2;
            auto [M, N, R] = ellipsoid.principle_curvature_radius(Bm);

            double
                p2 = std::pow(ps, 2),
                p3 = std::pow(ps, 3),
                V2 = std::pow(V, 2),
                V4 = std::pow(V, 4),
                V6 = std::pow(V, 6),
                n4 = std::pow(n2, 2),
                t2 = std::pow(t, 2),
                cosB2 = std::pow(cosB, 2),
                cosB3 = std::pow(cosB, 3);

            this->r01 = N / ps * cosB;
            this->r21 = (N * cosB) / (24 * p3 * V4) * (1 + n2 - 9 * n2 * t2 + n4);
            this->r03 = -N / (24 * p3) * (cosB3 * t2);
            this->S10 = N / (ps * V2);
            this->S12 = N / (24 * p3 * V2) * cosB2 * (2 + 3 * t2 + 2 * n2);
            this->S30 = N / (8 * p3 * V6) * (n2 - t2 * n2);
            this->t01 = t * cosB;
            this->t21 = 1.0 / (24 * p2 * V4) * cosB * t * (2 + 7 * n2 + 9 * t2 * n2 + 5 * n4);
            this->t03 = 1.0 / (24 * p2) * cosB3 * t * (2 + t2 + 2 * n2);
        }
    };

    double _gauss_lemma_AmRad(double T, double dBs, double dLs)
    {
        if (dBs > 0 && dLs >= 0)
        {
            return T;
        }
        else if (dBs < 0 && dLs >= 0)
        {
            return std::numbers::pi - T;
        }
        else if (dBs <= 0 && dLs < 0)
        {
            return std::numbers::pi + T;
        }
        else if (dBs > 0 && dLs < 0)
        {
            return 2.0 * std::numbers::pi - T;
        }
        else if (dBs == 0 && dLs >= 0)
        {
            return std::numbers::pi / 2.0;
        }
        else
        {
            return std::numeric_limits<double>::signaling_NaN();
        }
    }

    Geodetic_solve_result Gauss_lemma_solver::solve(Latitude latitude, Longitude longitude, double s, Azimuth angle, kwargs args) const
    {
        GAA_distance_assert(s);

        double B = latitude.value(), L = longitude.value(), S = s;
        double converge_threshold = args._has_converge_threshold() ? args.converge_threshold() : 1e-5;

        auto [outer_M, outer_N, _unused_R] = ellipsoid.principle_curvature_radius(B);

        double dB0 = rho2 / outer_M * std::cos(angle.value());
        double dL0 = rho2 / outer_N * std::sin(angle.value()) * (1.0 / std::cos(B));
        double dA0 = dL0 * std::sin(B);

        double
            Bm = rad2sec(B) + dB0 / 2.0,
            Am = rad2sec(angle.value()) + dA0 / 2.0;

        double dB{}, dL{}, dA{}, dBp{}, dLp{}, dAp{};

        do
        {
            dBp = dB;
            dLp = dL;
            dAp = dA;

            auto lc = ellipsoid.lat_aux(Latitude(sec2rad(Bm)));
            auto [M, N, R] = ellipsoid.principle_curvature_radius(sec2rad(Bm));
            double
                N2 = std::pow(N, 2),
                V = lc.v,
                V2 = std::pow(V, 2),
                S2 = std::pow(S, 2),
                t = lc.t,
                t2 = std::pow(t, 2),
                n2 = lc.nu_2,
                n4 = std::pow(n2, 2),
                p = rho2,
                cosA = std::cos(sec2rad(Am)),
                cosA2 = std::pow(cosA, 2),
                sinA = std::sin(sec2rad(Am)),
                sinA2 = std::pow(sinA, 2),
                secB = 1.0 / std::cos(sec2rad(Bm));

            dB = V2 / N * p * S * cosA * (1 + S2 / (24 * N2) * (sinA2 * (2 + 3 * t2 + 3 * n2 * t2) + 3 * n2 * cosA2 * (-1 + t2 - n2 - 4 * t2 * n2)));
            dL = p / N * S * secB * sinA * (1 + S2 / (24 * N2) * (sinA2 * t2 - cosA2 * (1 + n2 - 9 * t2 * n2 + n4)));
            dA = p / N * S * sinA * t * (1 + S2 / (24 * N2) * (cosA2 * (2 + 7 * n2 + 9 * t2 * n2 + 5 * n4) + sinA2 * (2 + t2 + 2 * n2)));

            Bm = rad2sec(B) + dB / 2.0;
            Am = rad2sec(angle.value()) + dA / 2.0;
        } while (
            !((std::abs(dB - dBp) < converge_threshold) &&
              (std::abs(dL - dLp) < converge_threshold) &&
              (std::abs(dA - dAp) < converge_threshold)));

        double L_tar = L + sec2rad(dL);
        double B_tar = B + sec2rad(dB);
        double d180s = 180 * 3600, d180r = deg2rad(180);
        double rangle = angle.value() + sec2rad(dA + (angle.value() < d180r ? 1 : -1) * d180s); // <-- units
        return Geodetic_solve_result{
            .latitude = Latitude(B_tar),
            .longitude = Longitude(L_tar),
            .rangle = Azimuth(rangle),
            .ellipsoid = ellipsoid};
    }

    Geodetic_rsolve_result Gauss_lemma_solver::rsolve(Latitude lat1, Longitude lon1, Latitude lat2, Longitude lon2) const
    {
        double B1 = lat1.value(), B2 = lat2.value(), L1 = lon1.value(), L2 = lon2.value();
        double
            dLs = rad2sec(L2 - L1),
            dBs = rad2sec(B2 - B1);
        double Bm = (B1 + B2) / 2.0;

        _gauss_lemma_coeff_t coeff_solver(Bm, ellipsoid);

        double
            U = coeff_solver.u(dLs, dBs),
            V = coeff_solver.v(dLs, dBs),
            dAs = coeff_solver.deltaAs(dLs, dBs);

        double
            c = std::abs(V / U),
            T = (std::abs(dBs) >= std::abs(dLs)) ? std::atan(U / V) : std::numbers::pi / 4.0 + std::atan((1.0 - c) / (1.0 + c)),
            Am_rad = _gauss_lemma_AmRad(T, dBs, dLs),
            Am = rad2sec(Am_rad);

        double
            S = (U / std::sin(Am_rad) + V / std::cos(Am_rad)) / 2.0,
            A12 = Am - dAs / 2.0,
            d180s = 180 * 3600,
            A21 = Am + dAs / 2.0 + (A12 < d180s ? 1 : -1) * d180s;
        return Geodetic_rsolve_result{
            .angle = Azimuth(sec2rad(A12)),
            .rangle = Azimuth(sec2rad(A21)),
            .s = S,
            .ellipsoid = ellipsoid};
    }
}