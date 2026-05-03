#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>

int main()
{
    gaa::rinex::v302::Nav_ephemeris_bds eph;

    eph.sqrtA = 0.515365263176e04;
    eph.e_Eccentricity = 0.678421219345e-02;
    eph.M0 = -0.290282040486e00;
    eph.Toe = 0.72e4;
    eph.year = std::chrono::year(1997);
    eph.month = std::chrono::month(11);
    eph.day = std::chrono::day(9);
    eph.hour = std::chrono::hours(2);
    eph.minute = std::chrono::minutes(0);
    eph.second = std::chrono::seconds(0);
    eph.omega = -0.258419417299e01;
    eph.OMEGA0 = -0.137835982556e01;
    eph.OMEGA_DOT = -0.819426989566e-08;
    eph.i0 = 0.958512160302e00;
    eph.IDOT = -0.253939149013e-09;
    eph.Cus = 0.912137329578e-05;
    eph.Cuc = 0.189989805222e-06;
    eph.Crs = 0.406250000000e01;
    eph.Crc = 0.201875000000e03;
    eph.Cis = -0.949949026108e-07;
    eph.Cic = 0.130385160446e-07;

    // t_k: float = julian_day(Y, M, D, H, Min, Sec) - gps_julian_day(931, t_oe)
    // print("tk = %.12f s" % (t_k * SEC_PER_DAY))

    auto [x, y, z] = gaa::sv_pos_from_broadcast(eph, 7200);
}