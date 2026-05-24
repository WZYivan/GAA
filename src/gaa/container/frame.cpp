#include <ranges>

#include <gaa/container/frame.hpp>

namespace gaa {
Indices const &Frame::indices() const { return m_indices; }

String Frame::glimpse() const {
  String str;
  auto out = std::back_inserter(str);

  this->visit_columns(make_visitor([&]<class T>(T const &col) {
    using Decay = std::decay_t<T>;
    static_assert(is_vector<Decay>);
    for (auto const &v : col) {
      std::format_to(out, "{}, ", v);
      if (str.size() >= 96) {
        str.resize(96);
        break;
      }
    }
    str.pop_back();
    str.pop_back();
    std::format_to(out, " ...\n");
  }));

  return str;
}
} // namespace gaa