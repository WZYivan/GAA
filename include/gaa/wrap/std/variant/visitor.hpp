#pragma once

#include <functional>

#include <gaa/wrap/std/variant/variant.hpp>

namespace gaa {
namespace variant {
template <Variable Var> class Visit_function {
public:
  bool is_null = true;
  using type = Variable_t<Var>;
  using type_fn = std::function<void(type const &)>;
  type_fn fn;

  template <class F>
    requires std::constructible_from<type_fn, F>
  Visit_function(F &&f) : is_null(false), fn(f) {}
  Visit_function() = default;
  ~Visit_function() = default;

  void operator()(Variant const &var) {
    if constexpr (Var == Variable::DEFAULT) {
      if (is_null) {
        return;
      } else {
        fn(var);
      }
    } else {
      type const &v = std::get<type>(var);
      if (is_null) {
        return;
      } else {
        fn(v);
      }
    }
  }
};

class Visitor {
private:
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  Visit_function<Variable::ENUM> m_fn_##ENUM;
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register

  Visit_function<Variable::DEFAULT> m_fn_DEFAULT;

public:
  ~Visitor() = default;
  Visitor() = default;

  template <class F> Visitor &case_of(Variable var, F &&f) {
    switch (var) {
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  case Variable::ENUM: {                                                       \
    m_fn_##ENUM = f;                                                           \
    break;                                                                     \
  }

#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
    case Variable::DEFAULT: {
      m_fn_DEFAULT = f;
      break;
    }
    default:
      gaa_fail("unreachable default case");
    }

    return *this;
  }

  void operator()(Variant const &var) {
    switch (var.variable()) {
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  case Variable::ENUM: {                                                       \
    m_fn_##ENUM.is_null ? m_fn_DEFAULT(var) : m_fn_##ENUM(var);                \
    break;                                                                     \
  }

#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
    default:
      gaa_fail("unreachable default case");
    }
  }

#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  Visitor &ENUM(Visit_function<Variable::ENUM> fn) {                           \
    return this->case_of(Variable::ENUM, fn);                                  \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register

  Visitor &Default(Visit_function<Variable::DEFAULT> fn) {
    return this->case_of(Variable::DEFAULT, fn);
  }
};
} // namespace variant

namespace make_visitor {
using namespace variant;

#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  inline Visitor ENUM(Visit_function<Variable::ENUM> fn) {                     \
    return Visitor{}.case_of(Variable::ENUM, fn);                              \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register

inline Visitor Default(Visit_function<Variable::DEFAULT> fn) {
  return Visitor{}.case_of(Variable::DEFAULT, fn);
}
} // namespace make_visitor
} // namespace gaa