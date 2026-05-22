#pragma once

#include <numeric>
#include <ranges>
#include <vector>

#include <gaa/core/keywords.hpp>

namespace gaa {
namespace _std {
template <class Derived> struct Vector_expr {
  Derived const &derived() const { return static_cast<Derived const &>(*this); }
  Derived &derived() { return static_cast<Derived &>(*this); }

  auto operator[](size_t idx) { return this->derived()[idx]; }
  auto operator[](size_t idx) const { return this->derived()[idx]; }
  size_t size() const { return this->derived().size(); }
};

template <class T>
concept Vector_Expr = requires(T const &t) {
  { t[0] };
  { t.size() } -> std::convertible_to<size_t>;
};

template <class T>
concept Non_Vector_Expr = !std::is_base_of_v<Vector_expr<T>, T>;

template <class V>
struct Vector : public Vector_expr<Vector<V>>, public std::vector<V> {
private:
  using Base = std::vector<V>;

public:
  using Base::Base;

  using Base::operator[];
  using Base::size;

  template <class Xpr> Vector(Xpr const &expr) {
    gaa_assert(expr.size() != std::numeric_limits<size_t>::max());
    for (auto i = 0uz; i != expr.size(); ++i) {
      this->emplace_back(expr[i]);
    }
  }

  template <class C> V dot(Vector<C> const &vec) const {
    gaa_assert(this->size() == vec.size());

    auto expr = std::views::zip_transform(
        [](auto const &v1, auto const &v2) { return v1 * v2; }, *this, vec);
    return std::accumulate(expr.begin(), expr.end(), static_cast<V>(0));
  }
};

template <Non_Vector_Expr V> struct Vector_constant;

template <class X> struct Vector_expr_storage {
  using type = std::add_lvalue_reference_t<std::add_const_t<X>>;
};

template <Non_Vector_Expr V> struct Vector_expr_storage<Vector_constant<V>> {
  using type = Vector_constant<V>;
};

template <Non_Vector_Expr V>
struct Vector_constant : public Vector_expr<Vector_constant<V>> {
private:
  using eval_result_t = V;

  V v;

public:
  Vector_constant(V a) : v(a) {}

  auto operator[](size_t i) { return v; }
  auto operator[](size_t i) const { return v; }
  size_t size() const { return std::numeric_limits<size_t>::max(); }

  eval_result_t eval() const { return v; }
};

template <class X>
using Vector_expr_storage_t = typename Vector_expr_storage<X>::type;

template <class X> using Expr_eval_result_t = typename X::eval_result_t;
} // namespace _std

using _std::Vector;

using Vecd = Vector<double>;
using Veci = Vector<int>;
using Vecs = Vector<std::string>;

#define GAA_EXPR_binary_op(OP_NAME, OP)                                        \
  namespace _std {                                                             \
  template <class X1, class X2>                                                \
  class Vector_##OP_NAME : public Vector_expr<Vector_##OP_NAME<X1, X2>> {      \
  public:                                                                      \
    using eval_result_t =                                                      \
        Vector<decltype(std::declval<X1>()[0] OP std::declval<X2>()[0])>;      \
                                                                               \
  private:                                                                     \
    Vector_expr_storage_t<X1> x1;                                              \
    Vector_expr_storage_t<X2> x2;                                              \
                                                                               \
  public:                                                                      \
    Vector_##OP_NAME(X1 const &a, X2 const &b) : x1(a), x2(b) {                \
      gaa_assert(x1.size() == x2.size() ||                                     \
                     (x1.size() == std::numeric_limits<size_t>::max() ||       \
                      x2.size() == std::numeric_limits<size_t>::max()),        \
                 "vector expr size mismatch: {} != {}", x1.size(), x2.size()); \
    }                                                                          \
    ~Vector_##OP_NAME() = default;                                             \
                                                                               \
    auto operator[](size_t idx) { return x1[idx] OP x2[idx]; }                 \
    auto operator[](size_t idx) const { return x1[idx] OP x2[idx]; }           \
    size_t size() const {                                                      \
      if (x1.size() != std::numeric_limits<size_t>::max()) {                   \
        return x1.size();                                                      \
      } else {                                                                 \
        return x2.size();                                                      \
      }                                                                        \
    }                                                                          \
                                                                               \
    eval_result_t eval() const {                                               \
      eval_result_t eval;                                                      \
      eval.reserve(this->size());                                              \
      for (auto i = 0uz; i != this->size(); ++i) {                             \
        eval.emplace_back((*this)[i]);                                         \
      }                                                                        \
      return eval;                                                             \
    }                                                                          \
  };                                                                           \
  }                                                                            \
  template <class X1, class X2>                                                \
  inline auto operator OP(_std::Vector_expr<X1> const &a,                      \
                          _std::Vector_expr<X2> const &b) {                    \
    return _std::Vector_##OP_NAME<X1, X2>(a.derived(), b.derived());           \
  }                                                                            \
  template <class X, _std::Non_Vector_Expr V>                                  \
  inline auto operator OP(_std::Vector_expr<X> const &x, V const &v) {         \
    return _std::Vector_##OP_NAME<X, _std::Vector_constant<V>>(x.derived(),    \
                                                               v);             \
  }                                                                            \
  template <_std::Non_Vector_Expr V, class X>                                  \
  inline auto operator OP(V const &v, _std::Vector_expr<X> const &x) {         \
    return _std::Vector_##OP_NAME<_std::Vector_constant<V>, X>(v,              \
                                                               x.derived());   \
  }

GAA_EXPR_binary_op(plus, +);
GAA_EXPR_binary_op(subtract, -);
GAA_EXPR_binary_op(multiply, *);
GAA_EXPR_binary_op(divide, /);
GAA_EXPR_binary_op(mod, %);
GAA_EXPR_binary_op(bit_and, &);
GAA_EXPR_binary_op(bit_or, |);
GAA_EXPR_binary_op(bit_xor, ^);

#undef GAA_EXPR_binary_op

#define GAA_EXPR_prefix_unary_op(OP_NAME, OP)                                  \
  namespace _std {                                                             \
  template <class X>                                                           \
  struct Vector_##OP_NAME : public Vector_expr<Vector_##OP_NAME<X>> {          \
  private:                                                                     \
    using eval_result_t = Vector<decltype(OP std::declval<X>()[0])>;           \
                                                                               \
    Vector_expr_storage_t<X> x;                                                \
                                                                               \
  public:                                                                      \
    Vector_##OP_NAME(X const &a) : x(a) {}                                     \
                                                                               \
    auto operator[](size_t i) { return OP x[i]; }                              \
    auto operator[](size_t i) const { return OP x[i]; }                        \
    size_t size() const { return x.size(); }                                   \
                                                                               \
    eval_result_t eval() const {                                               \
      eval_result_t eval;                                                      \
      eval.reserve(this->size());                                              \
      for (auto i = 0uz; i != this->size(); ++i) {                             \
        eval.emplace_back((*this)[i]);                                         \
      }                                                                        \
      return eval;                                                             \
    }                                                                          \
  };                                                                           \
  }                                                                            \
  template <class X> inline auto operator OP(_std::Vector_expr<X> const &x) {  \
    return _std::Vector_##OP_NAME<X>(x.derived());                             \
  }

GAA_EXPR_prefix_unary_op(negative, -);
GAA_EXPR_prefix_unary_op(positive, +);
GAA_EXPR_prefix_unary_op(logical_not, !);

#undef GAA_EXPR_prefix_unary_op
} // namespace gaa