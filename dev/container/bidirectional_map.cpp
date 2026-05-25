#include <print>

#include <gaa/container/bidirectional_map.hpp>
#include <gaa/core/keywords.hpp>

int main() {
  gaa::Bidirectional_map<std::string, int> bd;
  bd.insert_or_assign("mike", 1);

  gaa_assert(bd.at("mike") == 1);
  gaa_assert(bd.at(1) == "mike");
}