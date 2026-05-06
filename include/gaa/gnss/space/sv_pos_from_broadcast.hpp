#pragma once

#include <gaa/core/dataframe.hpp>
#include <gaa/core/time.hpp>
#include <gaa/geodesy/ellipsoids.hpp>
#include <gaa/gnss/space/sv_orbit.hpp>

namespace gaa {
extern std::tuple<double, double, double>
sv_pos_from_broadcast(Data_frame const &df, Data_frame_idx row, double t);
} // namespace gaa