#include <print>

#include <gaa/container/list.hpp>

int main() {
  gaa::List list;

  list["Mike"] = 1;
  list["pi"] = 3.14;
  list["trash"] = "meaningless";

  auto idx_of_pi = list.index_of("pi");
  list.erase("trash");
  gaa_assert(idx_of_pi == list.index_of("pi"));

  list["old_scores"] = {4, 2, 6};
  list["new_scores"] = {1.23, 2.2, 6.7};

  list.erase("old_scores");
  std::println("{}", list.glimpse());

  list["is_scalar"] =
      list.valid_indices() | std::views::transform([&list](auto i) {
        return std::make_pair(list.name_of(i), list[i].info().is_scalar);
      }) |
      std::ranges::to<std::map<std::string, bool>>();
  list.garbage_collect();

  list["sublist"] = gaa::List();
  std::println("{}", list.glimpse());
}