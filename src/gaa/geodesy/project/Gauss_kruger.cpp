#include <gaa/geodesy/project/Gauss_kruger.hpp>
#include <gaa/core/math.hpp>

namespace gaa
{
    Gauss_kruger_project Gauss_kruger::project(double interval) const
    {
        return Gauss_kruger_project(interval, -1);
    }
    Gauss_kruger_project Gauss_kruger::project(double interval, int band) const
    {
        return Gauss_kruger_project(interval, band);
    }

    bool Gauss_kruger_project::is_virtual() const
    {
        return this->band() == -1;
    }

    bool Gauss_kruger_project::operator==(Gauss_kruger_project const &other) const
    {
        return this->interval() == other.interval() && this->band() == other.band();
    }

    bool Gauss_kruger_project::operator!=(Gauss_kruger_project const &other) const
    {
        return !(*this == other);
    }

    Gauss_kruger_project::Gauss_kruger_project(double val, int band)
        : m_band(band)
    {
        if (gaa::within_abs(val, 6.0))
        {
            m_id = Identifier::Six;
        }
        else if (gaa::within_abs(val, 3.0))
        {
            m_id = Identifier::Three;
        }
        else
        {
            gaa_assert(false, "construct `Gauss_kruger_project` failed, interval should be in [6.0, 3.0]");
        }
    }

    Gauss_kruger_project Gauss_kruger_project::operator()(int band) const
    {
        return Gauss_kruger_project(this->interval(), band);
    }

    Gauss_kruger_project_coordinate Gauss_kruger_project::coordinate(double x, double y, int b, Ellipsoid const &e) const
    {
        return Gauss_kruger_project_coordinate(x, y, b, *this, e);
    }

    Gauss_kruger_project::Identifier Gauss_kruger_project::interval() const
    {
        return this->m_id;
    }

    int Gauss_kruger_project::band() const
    {
        return this->m_band;
    }

    int Gauss_kruger_project::band_of(double lon) const
    {
        GAA_longitude_assert(lon);

        switch (m_id)
        {
        case Identifier::Six:
        {

            return int(lon / deg2rad(6.0)) + 1;
            break;
        }
        case Identifier::Three:
        {
            double
                rad_d3 = deg2rad(3.0),
                rad_d1_5 = rad_d3 / 2.0;
            return int(lon / rad_d3) + (std::fmod(
                                            lon,
                                            rad_d3) > rad_d1_5
                                            ? 1
                                            : 0);
            break;
        }

        default:
        {
            gaa_assert(false, "unreachable default case");
            break;
        }
        }
    }

    double Gauss_kruger_project::center_meridian(double band) const
    {
        switch (this->interval())
        {
        case Identifier::Six:
        {
            return deg2rad(6 * band - 3);
            break;
        }
        case Identifier::Three:
        {
            return deg2rad(3 * band);
            break;
        }
        default:
        {
            gaa_assert(false, "unreachable default case");
            break;
        }
        }
    }

    Gauss_kruger_project_coordinate Gauss_kruger_project::eval(Geodetic_coordinate const &gc) const
    {
        double L = gc.longitude.value();
        double B = gc.latitude.value();
        auto const &ellipsoid = gc.ellipsoid;
        auto aux = ellipsoid.lat_aux(gc.latitude);

        double t = aux.t,
               t2 = std::pow(t, 2),
               t4 = std::pow(t, 4),
               n2 = aux.nu_2,
               n4 = std::pow(n2, 2);
        double p = rho2,
               p2 = std::pow(p, 2),
               p3 = std::pow(p, 3),
               p4 = std::pow(p, 4),
               p5 = std::pow(p, 5),
               p6 = std::pow(p, 6);
        double sinB = std::sin(B),
               cosB = std::cos(B),
               cosBp3 = std::pow(cosB, 3),
               cosBp5 = std::pow(cosB, 5);
        int band = m_band == -1 ? this->band_of(L) : m_band;
        double l_c = this->center_meridian(band),
               l_c_s = rad2sec(l_c) /*seconds below*/,
               l_s = rad2sec(L),
               dl_s = l_s - l_c_s,
               l = dl_s,
               l2 = std::pow(dl_s, 2),
               l3 = std::pow(dl_s, 3),
               l4 = std::pow(dl_s, 4),
               l5 = std::pow(dl_s, 5),
               l6 = std::pow(dl_s, 6);
        auto [M, N, R] = ellipsoid.principle_curvature_radius(B);
        double X = ellipsoid.meridian_arc_length(B);

        double x = X +
                   N / (2 * p2) * sinB * cosB * l2 +
                   N / (24 * p4) * sinB * cosBp3 * (5 - t2 + 9 * n2 + 4 * n4) * l4 +
                   N / (720 * p6) * sinB * cosBp5 * (61 - 58 * t2 + t4) * l6;
        double y = N / p * cosB * l +
                   N / (6 * p3) * cosBp3 * (1 - t2 + n2) * l3 +
                   N / (120 * p5) * cosBp5 * (5 - 18 * t2 + t4 + 14 * n2 - 58 * n2 * t2) * l5;
        return this->coordinate(x, y, band, ellipsoid);
    }

    Geodetic_coordinate Gauss_kruger_project::reval(Gauss_kruger_project_coordinate const &pc) const
    {
        double x = pc.x, y = pc.y;
        double band = pc.band;
        auto const &ellipsoid = pc.ellipsoid;
        auto const &project = pc.project;

        double Bf = ellipsoid.meridian_arc_bottom_latitude(x);
        auto [Mf, Nf, Rf] = ellipsoid.principle_curvature_radius(Bf);
        auto aux = ellipsoid.lat_aux(Latitude(Bf));

        double tf = aux.t, nf2 = aux.nu_2;
        double tf2 = std::pow(tf, 2),
               tf4 = std::pow(tf, 4),
               nf4 [[maybe_unused]] = std::pow(nf2, 2);
        double Nf3 = std::pow(Nf, 3),
               Nf5 = std::pow(Nf, 5);
        double cosBf = std::cos(Bf);
        double y2 = std::pow(y, 2),
               y3 = std::pow(y, 3),
               y4 = std::pow(y, 4),
               y5 = std::pow(y, 5),
               y6 = std::pow(y, 6);

        double B = Bf -
                   tf / (2.0 * Mf * Nf) * y2 +
                   tf / (24 * Mf * Nf3) * (5 + 3 * tf2 + nf2 - 9 * nf2 * tf2) * y4 -
                   tf / (720 * Mf * Nf5) * (61 + 90 * tf2 + 45 * tf4) * y6;
        double dl = 1.0 / (Nf * cosBf) * y -
                    1.0 / (6.0 * Nf3 * cosBf) * (1 + 2 * tf2 + nf2) * y3 +
                    1.0 / (120 * Nf5 * cosBf) * (5 + 28 * tf2 + 24 * tf4 + 6 * nf2 + 8 * nf2 * tf2) * y5;
        double Lc = project.center_meridian(band);

        return ellipsoid.coordinate(B, Lc + dl);
    }

    Gauss_kruger const gauss_kruger = {};

    double Gauss_kruger_project_coordinate::zone_y() const
    {
        return this->y + 500'000.0 + this->band * 100'000'0;
    }

}