#include <gaa/wrap/std/variant/variant.hpp>
#include <gaa/wrap/std/variant/visitor.hpp>

namespace gaa::variant {
Variable Variant::variable() const { return m_var; }

Format Format::json() {
  return Format{
      .scalar{.begin{""}, .end{""}},
      .vector{.begin{"["}, .end{"]"}, .sep{","}},
      .map{.begin{"{"}, .end{"}"}, .sep{","}},
      .pair{.begin{"\""}, .end{""}, .sep{"\":"}},
      .unformattable{.begin{""}, .end{""}, .content{"\"unformattable\""}}};
}

Format Format::list() { return Format{}; }

Format const fmt::list = Format::list();
Format const fmt::json = Format::json();
} // namespace gaa::variant