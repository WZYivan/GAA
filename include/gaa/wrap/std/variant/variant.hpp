#pragma once

#include <string>
#include <variant>
#include <vector>

#include <gaa/core/keywords.hpp>
#include <gaa/core/pp.hpp>
#include <gaa/wrap/std/concepts.hpp>
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
enum class Variable : int {
  Unknown = 0,
#include <gaa/wrap/std/variant/enums.hpp>
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

  Variant &operator=(char const *cstr) {
    this->operator=(std::string(cstr));
    return *this;
  }
  template <class T> Variant &operator=(std::initializer_list<T> il) {
    if constexpr (!Is_Pair<T>) {
      this->operator=(std::vector(il));
    } else {
      this->operator=(std::map(il));
    }
    return *this;
  }

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

template <class T> struct Variable_of {
  constexpr static Variable value = Variable::Unknown;
};
template <class T>
inline constexpr Variable variable_of_v = Variable_of<T>::value;

#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  template <> struct Variable_of<GAA_PP_STRIP_PARAM TYPE> {                    \
    constexpr static Variable value = Variable::ENUM;                          \
  };
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
} // namespace variant

inline std::string enum2str(variant::Variable v) {
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
}

inline bool is_scalar(variant::Variable v) {
  auto x = std::to_underlying(v);
  return x != 0 && (x - 1) % 4 == 0;
}
inline bool is_scalar(variant::Variant v) { return is_scalar(v.variable()); }

inline bool is_vector(variant::Variable v) {
  auto x = std::to_underlying(v);
  return x != 0 && (x - 1) % 4 == 1;
}
inline bool is_vector(variant::Variant v) { return is_vector(v.variable()); }

inline bool is_map(variant::Variable v) {
  auto x = std::to_underlying(v);
  return x != 0 && ((x - 1) % 4 >= 2);
}
inline bool is_map(variant::Variant v) { return is_map(v.variable()); }

} // namespace gaa