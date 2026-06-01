#include <gaa/container/frame.hpp>
#include <print>

int main() {
  gaa::Frame df(5);

  std::vector<int> id_col = {1, 2, 3, 4, 5};
  df.new_col("id", id_col);

  std::vector<double> val_col = {1.1, 2.2, 3.3, 4.4, 5.5};
  df.new_col("value", val_col);

  std::vector<std::string> name_col = {"A", "B", "C", "D", "E"};
  df.new_col("name", name_col);

  std::println("{}", df.glimpse());

  auto &id_vec = df.col<int>("id");
  id_vec[2] = 100;

  std::println("\nAfter modification:");
  std::println("{}", df.glimpse());

  df.erase_col("value");
  std::println("\nAfter erasure:");
  std::println("{}", df.glimpse());

  df.garbage_collect();
  std::println("\nAfter gc:");
  std::println("{}", df.glimpse());

  auto row0 = df.row(0);
  gaa_assert(row0.at<int>(0) == 1);
  row0.at<int>(0) = 69;
  df.rename_col("id", "new_id");
  std::println("\nAfter modification (row):");
  std::println("{}", df.glimpse());

  return 0;
}