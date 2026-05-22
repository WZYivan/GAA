#include <gaa/io/rinex3/nav.hpp>

int main() {
  auto r3 =
      gaa::read_rinex3_nav("/home/azusa/file/project/GAA/data/gths135a.18f");
  std::cout << r3.table().glimpse();
}