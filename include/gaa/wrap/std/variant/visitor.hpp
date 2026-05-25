#pragma once

#include <functional>

#include <gaa/wrap/std/concepts.hpp>
#include <gaa/wrap/std/format.hpp>
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

/// brief:
/// runtime switch invoker for Variant
class Visitor {
private:
/// x-macros begin
/// x is [Double, String, ...]
/// expand like
/// Visit_function<Variable::Double> m_fn_Double;
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  Visit_function<Variable::ENUM> m_fn_##ENUM;
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
  /// x-macros end

  Visit_function<Variable::DEFAULT> m_fn_DEFAULT;

public:
  ~Visitor() = default;
  Visitor() = default;

  template <class F> Visitor &case_of(Variable var, F &&f) {
    switch (var) {

      /// x-macros begin
      /// expand like
      /// case Variable::String: { m_fn_String = f; break; }
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  case Variable::ENUM: {                                                       \
    m_fn_##ENUM = f;                                                           \
    break;                                                                     \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
      /// x-macros end

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
  Visitor &ENUM(Visit_function<Variable::ENUM> fn) {                           \
    return this->case_of(Variable::ENUM, fn);                                  \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
  /// x-macros end

  Visitor &Default(Visit_function<Variable::DEFAULT> fn) {
    return this->case_of(Variable::DEFAULT, fn);
  }
};

/// brief:
/// using in gaa::make_visitor::format_to
struct Format {
  struct {
    std::string begin = "", end = "";
  } scalar;

  struct {
    std::string begin = "[", end = "]", sep = " ";
  } vector;

  struct {
    std::string begin = "[", end = "]", sep = " ";
  } map;

  struct {
    std::string begin = "<", end = ">", sep = ":";
  } pair;

  struct {
    std::string begin = "<", end = ">", content = "unformattable";
  } unformattable;

  static Format json();
  static Format list();
};

namespace fmt {
extern Format const json, list;
}
} // namespace variant

namespace make_visitor {
/// x-macros begin
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  inline variant::Visitor ENUM(                                                \
      variant::Visit_function<variant::Variable::ENUM> fn) {                   \
    return variant::Visitor{}.case_of(variant::Variable::ENUM, fn);            \
  }
#include <gaa/wrap/std/variant/enums.hpp>
#undef GAA_VARIANT_register
/// x-macros end

inline variant::Visitor
Default(variant::Visit_function<variant::Variable::DEFAULT> fn) {
  return variant::Visitor{}.case_of(variant::Variable::DEFAULT, fn);
}

template <class T> inline variant::Visitor unwrap_to(T &ref) {
/// x-macros begin
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  ENUM([&ref]<class V>(V const &v) {                                           \
    if constexpr (std::same_as<T, V>) {                                        \
      ref = v;                                                                 \
    } else {                                                                   \
      gaa_fail("type mismatch");                                               \
    }                                                                          \
  }).
  return make_visitor::
#include <gaa/wrap/std/variant/enums.hpp>
      Default([](auto const &v) {
        gaa_fail("unreachable default case of Visitor");
      });
#undef GAA_VARIANT_register
  /// x-macros end
}

template <class OutIterator>
inline variant::Visitor
format_to(OutIterator &out, variant::Format const &fmt = variant::fmt::list) {
  constexpr auto do_format_to = []<class OutIt, class V>(
                                    OutIt &out, V const &v,
                                    variant::Format const &fmt) {
    if constexpr (std::same_as<std::decay_t<V>, std::string>) {
      std::format_to(out, "{}\"{}\"{}", fmt.scalar.begin, v, fmt.scalar.end);
    } else if constexpr (std::ranges::range<V>) {
      if constexpr (Is_Vector<V> &&
                    std::formattable<std::ranges::range_value_t<V>, char>) {
        std::format_to(out, "{}{}", fmt.vector.begin, *v.begin());
        for (auto it = v.begin() + 1; it != v.end(); ++it) {
          std::format_to(out, "{}{}", fmt.vector.sep, *it);
        }
        std::format_to(out, "{}", fmt.vector.end);

      } else if constexpr (Is_Map<V>) {
        if constexpr (std::formattable<typename V::key_type, char> &&
                      (std::formattable<typename V::mapped_type, char> ||
                       std::same_as<typename V::mapped_type, bool>)) {
          auto it = v.begin();
          std::format_to(out, "{}{}{}{}{}{}", fmt.map.begin, fmt.pair.begin,
                         it->first, fmt.pair.sep, it->second, fmt.pair.end);
          ++it;
          for (; it != v.end(); ++it) {
            std::format_to(out, "{}{}{}{}{}{}", fmt.map.sep, fmt.pair.begin,
                           it->first, fmt.pair.sep, it->second, fmt.pair.end);
          }
          std::format_to(out, "{}", fmt.map.end);
        } else {
          std::format_to(out, "{}{}{}", fmt.unformattable.begin,
                         fmt.unformattable.content, fmt.unformattable.end);
        }

      } else {
        std::format_to(out, "{}{}{}", fmt.unformattable.begin,
                       fmt.unformattable.content, fmt.unformattable.end);
      }
    } else if constexpr (std::formattable<V, char>) {
      std::format_to(out, "{}{}{}", fmt.scalar.begin, v, fmt.scalar.end);
    } else {
      std::format_to(out, "{}{}{}", fmt.unformattable.begin,
                     fmt.unformattable.content, fmt.unformattable.end);
    }
  };

/// x-macros begin
#define GAA_VARIANT_register(TYPE, ENUM)                                       \
  ENUM([&out, &do_format_to, &fmt]<class V>(V const &v) {                      \
    do_format_to(out, v, fmt);                                                 \
  }).
  return make_visitor::
#include <gaa/wrap/std/variant/enums.hpp>
      Default([]<class V>(V const &) { gaa_fail("unreachable default case"); });
#undef GAA_VARIANT_register
  /// x-macros end
}
} // namespace make_visitor
} // namespace gaa