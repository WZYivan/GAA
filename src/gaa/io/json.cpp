// #include <filesystem>
// #include <fstream>

// #include <gaa/io/json.hpp>

// namespace fs = std::filesystem;

// namespace gaa {
// void write_json(std::string const &fname, List const &list, kwargs args) {
//   fs::path fpath{fname};
//   if (fpath.has_parent_path() && !fs::exists(fpath.parent_path())) {
//     fs::create_directories(fpath.parent_path());
//   }
//   std::ofstream ofs{fpath, std::ios::out};
//   gaa_assert(ofs, "bad ofstream using \'{}\'", fname);
//   write_json(ofs, list, args);
// }

// void write_json(std::ostream &os, List const &list, kwargs args) {
//   gaa_assert(os.good());
//   GAA_ARG_OR(args, stream_precision, 5);
//   os << std::setprecision(stream_precision);
//   std::ostream_iterator<char> out(os);
//   auto formatter = make_visitor::format_to(out, variant::Format::json());

//   auto r = list.valid_indices();
//   auto it = r.begin();

//   os << "{";
//   std::format_to(out, "\"{}\":", list.name_of(*it));
//   formatter(list[*it]);
//   ++it;
//   for (; it != r.end(); ++it) {
//     std::format_to(out, ",\"{}\":", list.name_of(*it));
//     formatter(list[*it]);
//   }
//   os << "}";
// }
// } // namespace gaa