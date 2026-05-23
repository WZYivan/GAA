#pragma once

#include <gaa/wrap/std/vectorize/expr.hpp>

namespace gaa::vectorize {

template <class V> struct Copy_input {
  constexpr static bool value =
      (std::is_arithmetic_v<V> || std::same_as<V, bool>);
};

template <class V> inline constexpr bool Copy_input_v = Copy_input<V>::value;

template <class V> struct Op_input {
  using type =
      std::conditional_t<Copy_input_v<V>, V,
                         std::add_lvalue_reference_t<std::add_const_t<V>>>;
};

template <class V> using Op_input_t = typename Op_input<V>::type;

struct Op_plus {
  template <class V1, class V2>
  static auto eval(Op_input_t<V1> a, Op_input_t<V2> b) {
    return a + b;
  }
};

struct Op_minus {
  template <class V1, class V2>
  static auto eval(Op_input_t<V1> a, Op_input_t<V2> b) {
    return a - b;
  }
};

struct Op_multiply {
  template <class V1, class V2>
  static auto eval(Op_input_t<V1> a, Op_input_t<V2> b) {
    return a * b;
  }
};

struct Op_divide {
  template <class V1, class V2>
  static auto eval(Op_input_t<V1> a, Op_input_t<V2> b) {
    return a / b;
  }
};

struct Op_neg {
  template <class V> static auto eval(Op_input_t<V> a) { return -a; }
};

struct Op_pos {
  template <class V> static auto eval(Op_input_t<V> a) { return +a; }
};

struct Op_not {
  template <class V> static auto eval(Op_input_t<V> a) { return !a; }
};

template <class X1, class X2, class Op>
struct Expr_binary_op : public Expr_base<Expr_binary_op<X1, X2, Op>> {
private:
  Expr_input_storage_t<X1> x1;
  Expr_input_storage_t<X2> x2;

public:
  Expr_binary_op(X1 const &a, X2 const &b) : x1(a), x2(b) {
    gaa_assert(!(a.valid_size() && b.valid_size()) || a.size() == b.size());
  }
  ~Expr_binary_op() = default;

  auto operator[](std::size_t i) const {
    return Op::template eval<decltype(std::declval<X1>()[0]),
                             decltype(std::declval<X2>()[0])>(x1[i], x2[i]);
  }

  std::size_t size() const { return x1.valid_size() ? x1.size() : x2.size(); }
};

template <class X, class Op>
struct Expr_unary_op : public Expr_base<Expr_unary_op<X, Op>> {
private:
  Expr_input_storage_t<X> x;

public:
  Expr_unary_op(X const &a) : x(a) {}
  ~Expr_unary_op() = default;

  auto operator[](std::size_t i) const {
    return Op::template eval<decltype(std::declval<X>()[0])>(x[i]);
  }

  std::size_t size() const { return x.size(); }
};
} // namespace gaa::vectorize