#include <gaa/core/keywords.hpp>
#include <gaa/gnss/rinex.hpp>

#define STR2ENUM(ENUM)         \
    if (s.compare(#ENUM) == 0) \
    {                          \
        return ENUM;           \
    }

namespace gaa::rinex
{
    std::string label_of(std::string const &s)
    {
        auto label = s.substr(60, s.length() - 61);
        return label;
    }

    std::string content_of(std::string const &s)
    {
        return s.substr(0, 60);
    }

    FileType file_type(char const &c)
    {
        switch (c)
        {
        case 'N':
            return FileType::Navigation;

        default:
            gaa_assert(false, "unreachable default case");
        }
    }

    SatelliteSystem satellite_system(char const &c)
    {
        switch (c)
        {
        case 'G':
            return SatelliteSystem::GPS;
        case 'R':
            return SatelliteSystem::GLONASS;
        case 'E':
            return SatelliteSystem::Galileo;
        case 'J':
            return SatelliteSystem::QZSS;
        case 'C':
            return SatelliteSystem::BDS;
        case 'S':
            return SatelliteSystem::SBAS_Payload;
        case 'M':
            return SatelliteSystem::Mixed;

        default:
            gaa_assert(false, "unreachable default case");
        }
    }

    TimeZone time_zone(std::string const &s)
    {
        STR2ENUM(UTC)
        STR2ENUM(LCL)
        {
            gaa_assert(false, std::format("unknown time zone `{}`", s));
        }
    }

    IonosphericCorrectionType ionospheric_correction_type(std::string const &s)
    {
        STR2ENUM(GAL)
        STR2ENUM(GPSA)
        STR2ENUM(GPSB)
        STR2ENUM(QZSA)
        STR2ENUM(QZSB)
        STR2ENUM(BDSA)
        STR2ENUM(BDSB)
        {
            gaa_assert(false, std::format("unknown ionospheric correction type `{}`", s));
        }
    }

    TimeCorrectionType time_correction_type(std::string const &s)
    {
        STR2ENUM(GAUT)
        STR2ENUM(GPUT)
        STR2ENUM(SBUT)
        STR2ENUM(GLUT)
        STR2ENUM(GPGA)
        STR2ENUM(GLGP)
        STR2ENUM(QZGP)
        STR2ENUM(QZUT)
        STR2ENUM(BDUT)
        {
            gaa_assert(false, std::format("unknown time correction type `{}`", s));
        }
    }

    UTCIdentifier utc_identifier(int x)
    {
        switch (x)
        {
        case 0:
            return Unknown;
        case 1:
            return UTC_NIST;
        case 2:
            return UTC_USNO;
        case 3:
            return UTC_SU;
        case 4:
            return UTC_BIPM;
        case 5:
            return UTC_Europe_Lab;
        case 6:
            return UTC_CRL;
        case 7:
            return UTC_NSTC_BDS;
        default:
            return Not_Assigned;
        }
    }
    AugmentationSystem augmentation_system(std::string const &s)
    {
        STR2ENUM(EGNOS)
        STR2ENUM(WAAS)
        STR2ENUM(MASA)
        {
            gaa_assert(false, std::format("unknown augmentation system `{}`", s));
        }
    }

    Ellipsoid const &spatial_ref_sys(SatelliteSystem sys)
    {
        switch (sys)
        {
        case BDS:
            return cgcs2000;

        default:
            gaa_assert(false, "unreachable default case");
        }
    }
}