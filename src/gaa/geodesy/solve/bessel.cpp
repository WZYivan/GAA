#include <cmath>

#include <gaa/core/math.hpp>
#include <gaa/geodesy/solve/bessel.hpp>

namespace gaa
{

    struct _bessel_formula_coeff_t
    {
        double A, B, C, alpha, beta, beta_prime;

        ~_bessel_formula_coeff_t() = default;

        _bessel_formula_coeff_t(double cosA0p2, Ellipsoid const &ellipsoid)
        {
            double
                e2 = ellipsoid.geometry().e1_2(),
                e2_2 = ellipsoid.geometry().e2_2(),
                b = ellipsoid.geometry().b(),
                k2 = e2_2 * cosA0p2,
                k4 = std::pow(k2, 2),
                k6 = std::pow(k2, 3),
                e4 = std::pow(e2, 2),
                e6 = std::pow(e2, 3),
                cosA0p4 = std::pow(cosA0p2, 2);

            this->A = b * (1 +
                           k2 / 4.0 -
                           3 * k4 / 64.0 +
                           5 * k6 / 256.0);
            this->B = b * (k2 / 8.0 -
                           k4 / 32.0 +
                           15 * k6 / 1024.0);
            this->C = b * (k4 / 128.0 - 3 * k6 / 512.0);
            this->alpha = (e2 / 2.0 + e4 / 8.0 + e6 / 16.0) -
                          (e4 / 16.0 + e6 / 16.0) * cosA0p2 +
                          (3 * e6 / 128.0) * cosA0p4;
            this->beta = (e4 / 32.0 + e6 / 32.0) * cosA0p2 -
                         (e6 / 64.0) * cosA0p4;
            this->beta_prime = 2 * this->beta / cosA0p2;
        }
    };

    double _bessel_adjust_lambda(double lambda, double sinA1, double tan_lambda)
    {
        double abs_lambda = std::abs(lambda);

        if (sinA1 > 0 && tan_lambda > 0)
        {
            return abs_lambda;
        }
        else if (sinA1 > 0 && tan_lambda < 0)
        {
            return deg2rad(180) - abs_lambda;
        }
        else if (sinA1 < 0 && tan_lambda < 0)
        {
            return -abs_lambda;
        }
        else if (sinA1 < 0 && tan_lambda > 0)
        {
            return abs_lambda - deg2rad(180);
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double _bessel_adjust_A2(double A2, double sinA1, double tanA2)
    {
        double absA2 = std::abs(A2);

        if (sinA1 < 0 && tanA2 > 0)
        {
            return absA2;
        }
        else if (sinA1 < 0 && tanA2 < 0)
        {
            return deg2rad(180) - absA2;
        }
        else if (sinA1 > 0 && tanA2 > 0)
        {
            return deg2rad(180) + absA2;
        }
        else if (sinA1 > 0 && tanA2 < 0)
        {
            return deg2rad(360) - absA2;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double _bessel_adjust_A1(double A1, double p, double q)
    {
        double absA1 = std::abs(A1);

        if (p > 0 && q > 0)
        {
            return absA1;
        }
        else if (p > 0 && q < 0)
        {
            return deg2rad(180) - absA1;
        }
        else if (p < 0 && q < 0)
        {
            return deg2rad(180) + absA1;
        }
        else if (p < 0 && q > 0)
        {
            return deg2rad(360) - absA1;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double _bessel_adjust_sigma(double sigma, double cos_sigma)
    {
        double abs_sigma = std::abs(sigma);

        if (cos_sigma > 0)
        {
            return abs_sigma;
        }
        else if (cos_sigma < 0)
        {
            return deg2rad(180) - abs_sigma;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    Geodetic_solve_result Bessel_solver::solve(double latitude, double longitude, double s, double angle) const
    {
        GAA_latitude_assert(latitude);
        GAA_longitude_assert(longitude);
        GAA_distance_assert(s);
        GAA_azimuth_assert(angle);

        auto lc = ellipsoid.lat_aux(latitude);
        double
            W1 = lc.w,
            e2 = ellipsoid.geometry().e1_2(),
            sinB1 = std::sin(latitude),
            cosB1 = std::cos(latitude),
            sinu1 = sinB1 * std::sqrt(1 - e2) / W1,
            cosu1 = cosB1 / W1,
            sinA1 = std::sin(angle),
            cosA1 = std::cos(angle),
            sinA0 = cosu1 * sinA1,
            sinA0p2 = std::pow(sinA0, 2),
            cosA0p2 = 1 - sinA0p2,
            cot_sigma1 = cosu1 * cosA1 / sinu1,
            cot_sigma1p2 = std::pow(cot_sigma1, 2),
            sin_2sigma1 = 2.0 * cot_sigma1 / (cot_sigma1p2 + 1),
            cos_2sigma1 = (cot_sigma1p2 - 1) / (cot_sigma1p2 + 1);

        _bessel_formula_coeff_t coeff(cosA0p2, ellipsoid);
        double
            A = coeff.A,
            B = coeff.B,
            C = coeff.C,
            alpha = coeff.alpha,
            beta = coeff.beta;

        double
            sigma0 = (s - (B + C * cos_2sigma1) * sin_2sigma1) / A,
            sin_2sigma0 = std::sin(2 * sigma0),
            cos_2sigma0 = std::cos(2 * sigma0),
            sin_2_sigma1_a_sigma0 = sin_2sigma1 * cos_2sigma0 + cos_2sigma1 * sin_2sigma0,
            cos_2_sigma1_a_sigma0 = cos_2sigma1 * cos_2sigma0 - sin_2sigma1 * sin_2sigma0,
            sigma = sigma0 + (B + 5 * C * cos_2_sigma1_a_sigma0) * sin_2_sigma1_a_sigma0 / A,
            sin_sigma = std::sin(sigma),
            cos_sigma = std::cos(sigma),
            delta = (alpha * sigma + beta * (sin_2_sigma1_a_sigma0 - sin_2sigma1)) * sinA0,
            sinu2 = sinu1 * cos_sigma + cosu1 * cosA1 * sin_sigma,
            sinu2p2 = std::pow(sinu2, 2),
            B2 = std::atan(
                sinu2 / (std::sqrt(1 - e2) * std::sqrt(1 - sinu2p2))),
            lambda = std::atan(
                (sinA1 * sin_sigma) / (cosu1 * cos_sigma - sinu1 * sin_sigma * cosA1)),
            tan_lambda = std::tan(lambda);
        lambda = _bessel_adjust_lambda(lambda, sinA1, tan_lambda);
        double
            L2 = longitude + lambda - delta,
            A2 = std::atan(
                cosu1 * sinA1 / (cosu1 * cos_sigma * cosA1 - sinu1 * sin_sigma)),
            tanA2 = std::tan(A2);
        A2 = _bessel_adjust_A2(A2, sinA1, tanA2);

        return Geodetic_solve_result{
            .latitude = B2,
            .longitude = L2,
            .rangle = A2,
            .ellipsoid = ellipsoid,
        };
    }

    Geodetic_rsolve_result Bessel_solver::rsolve(double lat1, double lon1, double lat2, double lon2, kwargs args) const
    {
        GAA_latitude_assert(lat1);
        GAA_latitude_assert(lat2);
        GAA_longitude_assert(lon1);
        GAA_longitude_assert(lon2);

        double converge_threshold = args._has_converge_threshold() ? args.converge_threshold() : 1e-5;

        double B1 = lat1, B2 = lat2, L1 = lon1, L2 = lon2;
        auto
            lc1 = ellipsoid.lat_aux(B1),
            lc2 = ellipsoid.lat_aux(B2);

        double
            W1 = lc1.w,
            W2 = lc2.w,
            sinB1 = std::sin(B1),
            cosB1 = std::cos(B1),
            sinB2 = std::sin(B2),
            cosB2 = std::cos(B2),
            e2 = ellipsoid.geometry().e1_2(),
            sqrt_1_s_e2 = std::sqrt(1 - e2),
            sinu1 = sinB1 * sqrt_1_s_e2 / W1,
            sinu2 = sinB2 * sqrt_1_s_e2 / W2,
            cosu1 = cosB1 / W1,
            cosu2 = cosB2 / W2,
            L = L2 - L1,
            a1 = sinu1 * sinu2,
            a2 = cosu1 * cosu2,
            b1 = cosu1 * sinu2,
            b2 = sinu1 * cosu2;

        double
            delta = 0,
            lambda = L + delta,
            A1,
            sigma,
            x,
            sinA0,
            delta_p,
            lambda_p;

        do
        {
            delta_p = delta;
            lambda_p = lambda;
            double
                cos_lambda = std::cos(lambda),
                sin_lambda = std::sin(lambda);

            double
                p = cosu2 * sin_lambda,
                q = b1 - b2 * cos_lambda;
            A1 = std::atan(p / q);
            A1 = _bessel_adjust_A1(A1, p, q);

            double
                sinA1 = std::sin(A1),
                cosA1 = std::cos(A1),
                sin_sigma = p * sinA1 + q * cosA1,
                cos_sigma = a1 + a2 * cos_lambda;
            sigma = std::atan(sin_sigma / cos_sigma);
            sigma = _bessel_adjust_sigma(sigma, cos_sigma);

            sinA0 = cosu1 * sinA1;
            double
                sinA0p2 = std::pow(sinA0, 2),
                cosA0p2 = 1 - sinA0p2;
            x = 2 * a1 - cosA0p2 * cos_sigma;

            _bessel_formula_coeff_t coeff(cosA0p2, ellipsoid);
            double
                alpha = coeff.alpha,
                beta_prime = coeff.beta_prime;

            delta = (alpha * sigma - beta_prime * x * sin_sigma) * sinA0;
            lambda = L + delta;

        } while (
            !(
                std::abs(delta - delta_p) < converge_threshold &&
                std::abs(lambda - lambda_p) < converge_threshold));

        double
            sinA0p2 = std::pow(sinA0, 2),
            cosA0p2 = 1 - sinA0p2,
            cosA0p4 = std::pow(cosA0p2, 2),
            x2 = std::pow(x, 2),
            cos_sigma = std::cos(sigma),
            sin_sigma = std::sin(sigma),
            sin_lambda = std::sin(lambda),
            cos_lambda = std::cos(lambda),
            y = (cosA0p4 - 2 * x2) * cos_sigma;

        _bessel_formula_coeff_t coeff(cosA0p2, ellipsoid);
        double
            A = coeff.A,
            B_pp = 2 * coeff.B / cosA0p2,
            C_pp = 2 * coeff.C / cosA0p4,
            S = A * sigma + (B_pp * x + C_pp * y) * sin_sigma,
            A2 = std::atan(
                cosu1 * sin_lambda / (b1 * cos_lambda - b2));
        double d180r = deg2rad(180);
        A2 = (A1 < d180r ? 1 : -1) * d180r + A2;

        return Geodetic_rsolve_result{
            .angle = A1,
            .rangle = A2,
            .s = S,
            .ellipsoid = ellipsoid};
    }
}