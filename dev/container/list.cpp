#include <iostream>
#include <print>

#include <gaa/container/list.hpp>

using namespace gaa::basic_types;

int main() {
  gaa::List list;

  list.append("name", "Mike");
  list.at<String>("name") = "Conner";

  list.append<Numerics>("scores", {1, 2, 3});

  std::cout << list.fmt();
}