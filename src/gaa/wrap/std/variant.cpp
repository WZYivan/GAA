#include <gaa/wrap/std/variant/variant.hpp>
#include <gaa/wrap/std/variant/visitor.hpp>

namespace gaa::variant {
Variable Variant::variable() const { return m_var; }

void Visitor::operator()(Variant const &var) {
  switch (var.variable()) {

    /// x-macros begin
    /// expand like
    /// case Variable::String: { m_fn_String.is_null ? : m_fn_DEFAULT(var) :
    /// m_fn_String(var); break; }
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  case Variable::ENUM: {                                                       \
    m_fn_##ENUM.is_null ? m_fn_DEFAULT(var) : m_fn_##ENUM(var);                \
    break;                                                                     \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
    /// x-macros end

  default:
    gaa_fail("unreachable default case");
  }
}

/// x-macros begin
/// expand like
/// Visitor &String(Visit_function<Variable::String> fn){ return
/// this->case_of(Variable::String, fn); }
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  Visitor &Visitor::ENUM(Visit_function<Variable::ENUM> fn) {                  \
    return this->case_of(Variable::ENUM, fn);                                  \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
/// x-macros end

Visitor &Visitor::Default(Visit_function<Variable::DEFAULT> fn) {
  return this->case_of(Variable::DEFAULT, fn);
}

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

namespace gaa {
std::string enum2str(variant::Variable v) {
  /// x-macros begin
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  case variant::Variable::ENUM: {                                              \
    return #ENUM;                                                              \
  }
  switch (v) {
#include <gaa/wrap/std/variant/enums.hpp>
  default:
    gaa_fail("unreachable default");
  }
#undef GAA_VARIANT_register
  /// x-macros end
}

namespace make_visitor {
/// x-macros begin
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  variant::Visitor ENUM(variant::Visit_function<variant::Variable::ENUM> fn) { \
    return variant::Visitor{}.case_of(variant::Variable::ENUM, fn);            \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
/// x-macros end
variant::Visitor
Default(variant::Visit_function<variant::Variable::DEFAULT> fn) {
  return variant::Visitor{}.case_of(variant::Variable::DEFAULT, fn);
}
} // namespace make_visitor
} // namespace gaa