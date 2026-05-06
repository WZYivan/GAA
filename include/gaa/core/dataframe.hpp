#pragma once

#include <DataFrame/DataFrame.h>

#include <gaa/core/keywords.hpp>

namespace gaa {
using Data_frame_idx = unsigned long;
using Data_frame = hmdf::StdDataFrame<Data_frame_idx>;

template <class T>
inline T df_at(Data_frame const &df, Data_frame_idx const &row,
               char const *col) {
  gaa_assert(df.has_column(col));
  auto const &column = df.get_column<T>(col);
  gaa_assert(column.size() > row);
  return column.at(row);
}
} // namespace gaa