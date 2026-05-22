#include <charconv>
#include <format>

#include <boost/algorithm/string.hpp>

#include <gaa/core/keywords.hpp>
#include <gaa/core/math.hpp>
#include <gaa/geodesy/map_index.hpp>

namespace gaa {
Map_index::Scale Map_index::scale_of(int scale) {
  switch (scale) {
  case 1'000'000:
    return A;
  case 500'000:
    return B;
  case 250'000:
    return C;
  case 100'000:
    return D;
  case 50'000:
    return E;
  case 25'000:
    return F;
  case 10'000:
    return G;
  case 5'000:
    return H;
  default:
    gaa_fail("unreachable default");
  }
}

#define CASE(ALPHA, TYPE, VALUE)                                               \
  case Map_index::ALPHA:                                                       \
    return TYPE(VALUE)

Latitude Map_index::lat_dif_of(Map_index::Scale scale) {
  switch (scale) {
    CASE(A, Latitude, dms(4, 0, 0));
    CASE(B, Latitude, dms(2, 0, 0));
    CASE(C, Latitude, dms(1, 0, 0));
    CASE(D, Latitude, dms(0, 20, 0));
    CASE(E, Latitude, dms(0, 10, 0));
    CASE(F, Latitude, dms(0, 5, 0));
    CASE(G, Latitude, dms(0, 2, 30));
    CASE(H, Latitude, dms(0, 1, 15));
  default:
    gaa_fail("unreachable default");
  }
}
Longitude Map_index::lon_dif_of(Map_index::Scale scale) {
  switch (scale) {
    CASE(A, Longitude, dms(6, 0, 0));
    CASE(B, Longitude, dms(3, 0, 0));
    CASE(C, Longitude, dms(1.5, 0, 0));
    CASE(D, Longitude, dms(0, 30, 0));
    CASE(E, Longitude, dms(0, 15, 0));
    CASE(F, Longitude, dms(0, 7, 30));
    CASE(G, Longitude, dms(0, 3, 45));
    CASE(H, Longitude, dms(0, 1, 52.5));
  default:
    gaa_fail("unreachable default");
  }
}
#undef CASE

std::string Map_index::fmt() const {
  constexpr auto major_row2chr = [](int row) -> char { return 'A' + row - 1; };
  return std::format("{:1c}{:2d}{:1c}{:03d}{:03d}",
                     major_row2chr(this->m_major_row), this->m_major_col,
                     static_cast<char>(this->m_scale), this->m_minor_row,
                     this->m_minor_col);
}

Map_index::Scale Map_index::scale() const { return this->m_scale; }
int Map_index::minor_row() const { return this->m_minor_row; }
int Map_index::minor_col() const { return this->m_minor_col; }
int Map_index::major_row() const { return this->m_major_row; }
int Map_index::major_col() const { return this->m_major_col; }
Latitude Map_index::lat_dif() const { return lat_dif_of(this->scale()); }
Longitude Map_index::lon_dif() const { return lon_dif_of(this->scale()); }
Latitude Map_index::north() const { return this->south() + this->lat_dif(); }
Latitude Map_index::south() const { return this->m_south; }
Longitude Map_index::east() const { return this->west() + this->lon_dif(); }
Longitude Map_index::west() const { return this->m_west; }

Map_index::Map_index(std::string const &cnt) {
  gaa_assert(cnt.size() - 4 == 6 || cnt.size() - 4 == 8);

  char mr = cnt[0];
  std::string mc = cnt.substr(1, 2);
  m_major_row = mr - 'A' + 1;
  {
    auto [ptr, ec] =
        std::from_chars(mc.data(), mc.data() + mc.size(), m_major_col);
    gaa_assert(ec == std::errc{}, "fail parseing:{}", mc);
  }

  m_scale = static_cast<Scale>(cnt[3]);

  int len = (cnt.size() - 4) / 2;
  std::string r = cnt.substr(4, len);
  std::string c = cnt.substr(4 + len, len);

  {
    int len = r.find_first_not_of('0');
    boost::replace_head(r, len, " ");
    boost::trim(r);
    auto [ptr, ec] =
        std::from_chars(r.data(), r.data() + r.size(), m_minor_row);
    gaa_assert(ec == std::errc{}, "fail parseing:{}", r);
  }

  {
    int len = c.find_first_not_of('0');
    boost::replace_head(c, len, " ");
    boost::trim(c);
    auto [ptr, ec] =
        std::from_chars(c.data(), c.data() + c.size(), m_minor_col);
    gaa_assert(ec == std::errc{}, "fail parseing:{}", c);
  }
}

Map_index::Map_index(int major_row, int major_col, Scale scale, int minor_row,
                     int minor_col)
    : m_major_row(major_row), m_major_col(major_col), m_minor_row(minor_row),
      m_minor_col(minor_col), m_scale(scale) {
  double b = deg2rad(m_major_row * 4) + rad(this->lat_dif()) * m_minor_row;
  double l = deg2rad(m_major_col * 6) + rad(this->lon_dif()) * m_minor_col;
  m_south = Latitude(b);
  m_west = Longitude(l);
}

Map_index::Map_index(Latitude const &lat, Longitude const &lon, int scale) {
  double B = rad2deg(rad(lat));
  double L = rad2deg(rad(lon));

  m_major_row = int(B / 4.0) + 1;
  m_major_col = int(L / 6.0) + 31;

  m_scale = scale_of(scale);

  double dB = rad2deg(rad(this->lat_dif()));
  double dL = rad2deg(rad(this->lon_dif()));
  m_minor_row = 4 / dB - int(std::fmod(B, 4) / dB) + 1;
  m_minor_col = int(std::fmod(L, 6) / dL) + 1;

  double b = deg2rad(m_major_row * 4) + rad(this->lat_dif()) * m_minor_row;
  double l = deg2rad(m_major_col * 6) + rad(this->lon_dif()) * m_minor_col;
  m_south = Latitude(b);
  m_west = Longitude(l);
}

bool Map_index::operator==(Map_index const &other) const {
  return m_major_col == other.major_col() && m_major_row == other.major_row() &&
         m_scale == other.scale() && m_minor_col == other.minor_col() &&
         m_minor_row == other.minor_row();
}
} // namespace gaa