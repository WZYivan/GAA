#include <fstream>
#include <print>

#include <gaa/core/keywords.hpp>
#include <gaa/gnss/rinex.hpp>

#define FILE "/mnt/d/.resource/gnss/gths135a.18f.txt"

int main()
{
    std::ifstream ifs{FILE};
    auto hdr = gaa::rinex::v302::parse_nav_hdr(ifs);

    gaa_assert(hdr.file_type == gaa::rinex::Navigation);

    auto dat = gaa::rinex::v302::parse_nav_dat_bds(ifs, hdr);

    return 0;
}