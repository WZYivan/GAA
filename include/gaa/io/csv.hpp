#pragma once

#include <gaa/core/table.hpp>

namespace gaa {
extern Table read_csv(std::string const &fname,
                      std::vector<Table_Storage_Flag> const &flags,
                      kwargs args = {});
extern Table read_csv(std::istream &is,
                      std::vector<Table_Storage_Flag> const &flags,
                      kwargs args = {});
extern Table read_csv_auto(std::string const &fname, kwargs args = {});
extern Table read_csv_auto(std::istream &is, kwargs args = {});

extern bool write_csv(std::ostream &os, Table const &table);
} // namespace gaa