#include <print>

#include <gaa/core/io/csv.hpp>
#include <gaa/core/table.hpp>
using namespace gaa;

int main() {
  Table_column<Tab_double> col0{1.1, 2.2, 3.3, 4.4, 5.5};
  Table_column<Tab_int> col1{7, 8, 9, 10};
  Table_column<Tab_string> col2{"A", "BC", "DEF"};

  Table table;
  table.push_back(std::move(col0));
  table.push_back(std::move(col1));
  table.push_back(std::move(col2));
  table.meta_ioa<Tab_string>("tab_name", "dev_table");

  gaa_assert(col0.empty());
  gaa_assert(col1.empty());
  gaa_assert(col2.empty());

  gaa_assert(std::addressof(table.at<Tab_double>(0)) ==
             std::addressof(table.at<Tab_double>("COL_0")));
  gaa_assert(table.at<Tab_string>(2).front() == "A");
  {
    auto &col0 = table.at<Tab_double>(0);
    auto size = col0.size();
    col0.push_back(5.5);
    gaa_assert(col0.size() == size + 1);
  }

  gaa_assert(table.meta_at<Tab_string>("tab_name") == "dev_table");
  table.meta_at<Tab_string>("tab_name") = "new_dev_table_name";
  gaa_assert(table.meta_at<Tab_string>("tab_name") == "new_dev_table_name");

  table.clear();
  gaa_assert(table.empty());

  Table csv =
      read_csv_auto("/home/azusa/file/project/GAA/data/gths135a.18f.dat.csv",
                    kw{}.separator(',').ignore_first_line(true));

  Table_row_ref row0 = csv.row(0);
  Table::column_name_type gnss_id =
      table.has_column("gnss_id") ? "gnss_id" : "COL_34";
  gaa_assert(row0.at<Tab_string>(gnss_id) == "beidou");
  row0.at<Tab_string>(gnss_id) = "gps";
  gaa_assert(row0.at<Tab_string>(gnss_id) == "gps");

  return EXIT_SUCCESS;
}