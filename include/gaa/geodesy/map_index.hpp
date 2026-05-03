#pragma once

#include <gaa/core/units.hpp>
#include <string>

namespace gaa {
class Map_index {
public:
  enum Scale : char { A = 'A', B, C, D, E, F, G, H };
  static Scale scale_of(int scale);
  static Latitude lat_dif_of(Scale scale);
  static Longitude lon_dif_of(Scale scale);

private:
  int m_major_row;
  int m_major_col;
  int m_minor_row;
  int m_minor_col;
  Latitude m_south = Latitude(0.0);
  Longitude m_west = Longitude(0.0);
  Scale m_scale;

public:
  ~Map_index() = default;
  Map_index(Latitude const &lat, Longitude const &lon, int scale);
  Map_index(std::string const &cnt);
  Map_index(int major_row, int major_col, Scale scale, int minor_row,
            int minor_col);

  Scale scale() const;

  Latitude north() const;
  Latitude south() const;
  Longitude east() const;
  Longitude west() const;

  Latitude lat_dif() const;
  Longitude lon_dif() const;

  int minor_row() const;
  int minor_col() const;
  int major_row() const;
  int major_col() const;

  std::string fmt() const;

  bool operator==(Map_index const &other) const;
};
} // namespace gaa