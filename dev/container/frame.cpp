#include <iostream>

#include <gaa/container/frame.hpp>

int main() {
  using namespace gaa;
  Frame frame;

  frame.append_column(Numerics{1, 2, 3});
  frame.append_column(Strings{"row 1", "row 2", "row 3", "row 4"});

  std::cout << frame.glimpse();
}