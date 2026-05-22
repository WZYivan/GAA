#include <gaa/io/rinex2/nav.hpp>

int main() {
  auto r2 =
      gaa::read_rinex2_nav("/home/azusa/file/project/GAA/data/000A0070.20n");
  std::cout << r2.table().glimpse();
}