#pragma once

#include <iostream>
#include <string>

#include <gaa/container/list.hpp>
#include <gaa/core/kw.hpp>

namespace gaa {
extern void write_json(std::ostream &os, List const &list, kwargs args = {});
extern void write_json(std::string const &fname, List const &list,
                       kwargs args = {});
} // namespace gaa