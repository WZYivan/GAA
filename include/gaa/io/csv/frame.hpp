#pragma once

#include <span>

#include <gaa/container/frame.hpp>
#include <gaa/core/kw.hpp>

namespace gaa {
extern Frame read_csv_to_frame(std::string const &fname,
                               std::span<Literal_Type const> literals,
                               kwargs args = {});
extern Frame read_csv_to_frame(std::istream &is,
                               std::span<Literal_Type const> literals,
                               kwargs args = {});
extern Frame read_csv_to_frame(std::string const &fname, kwargs args = {});
extern Frame read_csv_to_frame(std::istream &is, kwargs args = {});

extern void write_csv(std::string const &fname, Frame const &df,
                      kwargs args = {});
extern void write_csv(std::ostream &os, Frame const &df, kwargs args = {});
} // namespace gaa