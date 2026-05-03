#pragma once

#include <cmath>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace gaa {
extern bool within_abs(double x, double y, double epsilon = 1e-10);
extern bool within_abs_pct(double x, double y, double pct = 0.05);
extern double round(double x);
extern double cut(double x, int precision);
extern double deg2rad(double);
extern double min2rad(double);
extern double sec2rad(double);
extern double rad2deg(double);
extern double rad2min(double);
extern double rad2sec(double);
extern double dms(double, double, double = 0);
extern long long factorial(int n);
extern long long combination(int n, int m);
extern long long arrangement(int n, int m);
extern std::vector<long long> pascal_triangle_row(int row);
extern std::vector<double> difference(std::span<double const> vals, int degree);
extern double dot_product(std::span<double const> v1,
                          std::span<double const> v2);

extern double const rho0, rho1, rho2;

struct Dbl_vec_fn {
  template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, double>
  std::vector<double> operator()(R &&vals) const {
    gaa_assert(!vals.empty());
    std::vector<double> dbl;
    dbl.reserve(vals.size());
    using val_cref_t = std::add_lvalue_reference_t<
        std::add_const_t<std::ranges::range_value_t<R>>>;
    for (val_cref_t val : vals) {
      dbl.emplace_back(val);
    }
    return dbl;
  }

  template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, double>
  friend std::vector<double> operator|(R &&vals, Dbl_vec_fn const &fn) {
    return fn(vals);
  }
};

inline constexpr Dbl_vec_fn dbl_vec{};

using Interval = std::pair<int, int>;

} // namespace gaa