#include <gaa/gnss/rinex.hpp>
#include <print>

int main() {
  auto [hdr, dat] =
      gaa::read_rinex3("/home/azusa/file/project/GAA/data/gths135a.18f");
  std::println("> hdr \n{}", gaa::str(hdr));
  for (auto const &d : dat) {
    std::println("> dat \n{}", gaa::str(d));
  }
}