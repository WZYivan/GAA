#include <numbers>

#include <gaa/core/keywords.hpp>
#include <gaa/core/math.hpp>

namespace gaa {
bool within_abs(double x, double y, double epsilon) {
  return std::abs(x - y) < epsilon;
}

double round(double x) {
  double int_part;
  double frac_part = std::modf(x, &int_part);

  if (within_abs(frac_part, 0.5)) {
    if (within_abs(std::fmod(int_part, 2), 0)) {
      return int_part;
    } else {
      return int_part + (x >= 0 ? 1 : -1);
    }
  } else if (std::abs(frac_part) < 0.5) {
    return int_part;
  } else // (std::abs(frac_part) > 0.5)
  {
    return int_part + (x >= 0 ? 1 : -1);
  }
}

double cut(double x, int precision) {
  if (precision < 0) {
    return x;
  }

  if (precision == 0) {
    return round(x);
  }
  double scale = std::pow(10, precision);
  return round(x * scale) / scale;
}

double deg2rad(double d) { return d / 180.0 * std::numbers::pi; }

double min2rad(double m) { return m / (180.0 * 60.0) * std::numbers::pi; }

double sec2rad(double s) { return s / (180.0 * 3600.0) * std::numbers::pi; }

double rad2deg(double r) { return r / std::numbers::pi * 180.0; }

double rad2min(double r) { return rad2deg(r) * 60; }

double rad2sec(double r) { return rad2deg(r) * 3600; }

double dms(double d, double m, double s) {
  gaa_assert(m >= 0 && s >= 0, "only degree with sign is acceptable");
  bool neg = std::signbit(d);
  d = std::abs(d);
  double r = deg2rad(d) + min2rad(m) + sec2rad(s);
  return neg ? -r : r;
}

long long factorial(int n) {
  gaa_assert(n >= 0);
  long long result = 1;
  for (int i = 2; i <= n; ++i)
    result *= i;
  return result;
}

long long arrangement(int n, int m) {
  gaa_assert(n >= m && m >= 0);
  if (m == 0)
    return 1;
  long long result = 1;
  for (int i = 0; i < m; ++i)
    result *= (n - i);
  return result;
}

long long combination(int n, int m) {
  gaa_assert(n >= m && m >= 0);
  if (m > n - m)
    m = n - m;
  return arrangement(n, m) / factorial(m);
}

std::vector<long long> pascal_triangle_row(int row) {
  gaa_assert(row >= 0);
  std::vector<long long> vec;
  vec.reserve(row + 1);
  for (int c = 0; c <= row; ++c) {
    vec.emplace_back(combination(row, c));
  }
  return vec;
}

std::vector<double> difference(std::span<double const> vals, int degree) {
  gaa_assert(vals.size() > 0);
  gaa_assert(degree > 0);

  std::vector<double> vec(vals.begin(), vals.end());

  while (degree-- > 0) {
    int len = vec.size();
    for (int l = 0, r = 1; r != len; ++l, ++r) {
      vec[l] = vec[r] - vec[l];
    }
    vec.pop_back();
  }

  return vec;
}

double dot_product(std::span<double const> v1, std::span<double const> v2) {
  gaa_assert(v1.size() == v2.size());
  double dot = 0;
  for (int i = 0; i != v1.size(); ++i) {
    dot += v1[i] * v2[i];
  }
  return dot;
}

double const rho0 = 57.295'779'513'082'321'0, rho1 = 3'437.746'770'784'939'17,
             rho2 = 206'264.806'247'096'355;
} // namespace gaa