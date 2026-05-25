#include <gaa/io/json.hpp>

int main() {
  gaa::List json;
  json["name"] = "Mike";
  json["age"] = 20;
  json["scores"] = {23, 56, 87};
  json["ratio"] = {4.1, 3.22, 5.66};
  json["graduate"] = gaa::Type_of_variable<gaa::Variable::Map_Str_Integer>{
      {"Math", 100}, {"PE", 20}, {"English", 80}};
  gaa::write_json("/home/azusa/file/project/GAA/data/list.json", json,
                  gaa::mkarg::stream_precision(10));
  std::cout << "\n" << json.glimpse();
}