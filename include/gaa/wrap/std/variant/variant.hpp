#pragma once

#include <string>
#include <variant>
#include <vector>

#include <gaa/core/keywords.hpp>
#include <gaa/core/pp.hpp>
#include <gaa/wrap/std/variant/enums_fwd.hpp>

namespace gaa {
namespace variant {

struct _variant_place_holder {};
#define GAA_VARIANT_register(TYPE, ENUM) GAA_PP_STRIP_PARAM TYPE,
using _std_variant = ::std::variant<
#include <gaa/wrap/std/variant/enums.hpp>
    _variant_place_holder>;
#undef GAA_VARIANT_register

#define GAA_VARIANT_register(TYPE, ENUM) ENUM,
enum class Variable {
  Unknown,
#include <gaa/wrap/std/variant/enums.hpp>
  COUNT,
  DEFAULT
};
#undef GAA_VARIANT_register

class Variant : public _std_variant {
public:
  using base_t = _std_variant;

private:
  Variable m_var = Variable::Unknown;

public:
  Variable variable() const;

  ~Variant() = default;
  Variant(char const *cstr) : Variant(std::string(cstr)) {}
  template <class T>
  Variant(std::initializer_list<T> &&il)
      : Variant(std::vector<T>(std::move(il))) {}

#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  Variant(GAA_PP_STRIP_PARAM TYPE &&v) : base_t(v), m_var(Variable::ENUM) {    \
    gaa_assert(m_var != Variable::Unknown, "invalid construct");               \
  }                                                                            \
  Variant(GAA_PP_STRIP_PARAM TYPE const &v)                                    \
      : base_t(v), m_var(Variable::ENUM) {                                     \
    gaa_assert(m_var != Variable::Unknown, "invalid construct");               \
  }                                                                            \
  Variant &operator=(GAA_PP_STRIP_PARAM TYPE &&v) {                            \
    base_t::operator=(std::move(v));                                           \
    m_var = Variable::ENUM;                                                    \
    return *this;                                                              \
  }                                                                            \
  Variant &operator=(GAA_PP_STRIP_PARAM TYPE const &v) {                       \
    base_t::operator=(v);                                                      \
    m_var = Variable::ENUM;                                                    \
    return *this;                                                              \
  }

#include <gaa/wrap/std/variant/enums.hpp>

#undef GAA_VARIANT_register
};

template <Variable Var> struct Type_of {
  static_assert(false, "Unspecialized Variable");
};
template <> struct Type_of<Variable::DEFAULT> {
  using type = Variant;
};

template <Variable Var> using Variable_t = Type_of<Var>::type;

#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  template <> struct Type_of<Variable::ENUM> {                                 \
    using type = GAA_PP_STRIP_PARAM TYPE;                                      \
  };

#include <gaa/wrap/std/variant/enums.hpp>

#undef GAA_VARIANT_register
} // namespace variant
} // namespace gaa