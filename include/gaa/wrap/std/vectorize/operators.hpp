#pragma once

#include <gaa/wrap/std/vectorize/expr.hpp>
#include <gaa/wrap/std/vectorize/operation.hpp>

namespace gaa::vectorize::operators {
template <class L, class R> inline auto operator+(L const &a, R const &b) {
  return Expr_binary_op<Make_expr<L>, Make_expr<R>, Op_plus>(make_expr<L>(a),
                                                             make_expr<R>(b));
}

template <class L, class R> inline auto operator-(L const &a, R const &b) {
  return Expr_binary_op<Make_expr<L>, Make_expr<R>, Op_minus>(make_expr<L>(a),
                                                              make_expr<R>(b));
}

template <class L, class R> inline auto operator*(L const &a, R const &b) {
  return Expr_binary_op<Make_expr<L>, Make_expr<R>, Op_multiply>(
      make_expr<L>(a), make_expr<R>(b));
}

template <class L, class R> inline auto operator/(L const &a, R const &b) {
  return Expr_binary_op<Make_expr<L>, Make_expr<R>, Op_divide>(make_expr<L>(a),
                                                               make_expr<R>(b));
}

template <class R> inline auto operator-(R const &a) {
  return Expr_unary_op<Make_expr<R>, Op_neg>(make_expr<R>(a));
}

template <class R> inline auto operator!(R const &a) {
  return Expr_unary_op<Make_expr<R>, Op_not>(make_expr<R>(a));
}
} // namespace gaa::vectorize::operators