#pragma once

#include <concepts>

#include <gaa/core/keywords.hpp>

namespace gaa::vectorize {
template <class Derived> struct Expr_base {
public:
  Derived const &derived() const { return static_cast<Derived const &>(*this); }
  Derived &derived() { return static_cast<Derived &>(*this); }

  auto operator[](std::size_t i) const { return this->derived()[i]; }
  auto operator[](std::size_t i) { return this->derived()[i]; }

  auto size() const { return this->derived().size(); }
  bool valid_size() const {
    return this->size() != std::numeric_limits<std::size_t>::quiet_NaN();
  }

  template <class Container> void eval_to(Container &c) const {
    auto out = std::back_inserter(c);
    for (auto i = 0uz; i != this->size(); ++i) {
      *out = (*this)[i];
    }
  }

  template <template <class ValueType, class...> class Container = std::vector>
  auto eval() const {
    return this->eval<Container<decltype(std::declval<Expr_base>()[0])>>();
  }

  template <class C> C eval() const {
    C c;
    auto out = std::back_inserter(c);
    for (auto i = 0uz; i != this->size(); ++i) {
      *out = (*this)[i];
    }
    return c;
  }
};

template <class C>
concept Expr_Representable_Container = requires(C const &c) {
  { c.size() } -> std::convertible_to<std::size_t>;
  { c[0] };
};

template <class C>
concept Expr_Representable =
    Expr_Representable_Container<C> || std::is_arithmetic_v<C>;

template <Expr_Representable C>
using Expr_wrapper_storage_t =
    std::conditional_t<Expr_Representable_Container<C>,
                       std::add_lvalue_reference_t<std::add_const_t<C>>, C>;

template <class T> struct Register_expr {
  constexpr static bool value =
#ifdef GAA_VECTORIZE_REGISTER_ALL
      true
#else
      false
#endif
      ||
#ifdef GAA_VECTORIZE_REGISTER_VIEW
      std::ranges::view<T>
#else
      false
#endif
      ||
#ifdef GAA_VECTORIZE_REGISTER_ARITHMETIC
      std::is_arithmetic_v<T>
#else
      false
#endif
      || std::is_base_of_v<Expr_base<T>, T>;
};

#define GAA_VECTORIZE_ALLOW() constexpr static bool value = true
#define GAA_VECTORIZE_FORBID() constexpr static bool value = false

template <class T>
inline constexpr bool Allowed_to_be_wrapped_as_expr_v = Register_expr<T>::value;

template <Expr_Representable C>
  requires Allowed_to_be_wrapped_as_expr_v<C>
struct Expr_wrapper : public Expr_base<Expr_wrapper<C>> {
private:
  Expr_wrapper_storage_t<C> c;

public:
  ~Expr_wrapper() = default;
  Expr_wrapper(C const &in) : c(in) {};

  auto operator[](std::size_t i) const {
    if constexpr (Expr_Representable_Container<C>) {
      return (c)[i];
    } else {
      return c;
    }
  }
  auto operator[](std::size_t i) {
    if constexpr (Expr_Representable_Container<C>) {
      return (c)[i];
    } else {
      return c;
    }
  }

  auto size() const {
    if constexpr (Expr_Representable_Container<C>) {
      return c.size();
    } else {
      return std::numeric_limits<std::size_t>::quiet_NaN();
    }
  }
};

template <class X>
concept Non_Expr = !std::is_base_of_v<Expr_base<X>, X>;

template <class X>
using Make_expr = std::conditional_t<Non_Expr<X>, Expr_wrapper<X>, X>;

template <class X> decltype(auto) make_expr(X const &c) {
  return Make_expr<X>(c);
}

template <class X> struct Expr_input_storage {
  using type = X;
};

template <class X> using Expr_input_storage_t = Expr_input_storage<X>::type;
} // namespace gaa::vectorize

#ifdef GAA_VECTORIZE_REGISTER_VECTOR
#include <gaa/wrap/std/vectorize/regsiter/vector.hpp>
#endif

#ifdef GAA_VECTORIZE_REGISTER_ARRAY
#include <gaa/wrap/std/vectorize/regsiter/array.hpp>
#endif

#ifdef GAA_VECTORIZE_REGISTER_EIGEN_DENSE
#include <gaa/wrap/std/vectorize/regsiter/Eigen/Dense.hpp>
#endif