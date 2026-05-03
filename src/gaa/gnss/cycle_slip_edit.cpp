#include <boost/icl/interval_set.hpp>

#include <gaa/core/keywords.hpp>
#include <gaa/core/math.hpp>
#include <gaa/gnss/cycle_slip_edit.hpp>

namespace gaa {

Cycle_detect_result diff_detect(std::span<double const> vals, int degree,
                                kwargs args) {
  gaa_assert(degree > 0);
  gaa_assert(vals.size() > degree * 2);

  GAA_ARG_OR(args, lt_threshold, 0.2);

  auto diff = difference(vals, degree);
  auto pattern = pascal_triangle_row(degree - 1) | dbl_vec;
  double pattern_factor = dot_product(pattern, pattern);
  for (int i = 0; i != pattern.size(); ++i) {
    if (i % 2 != 0) {
      pattern[i] *= -1;
    }
  }

  Cycle_detect_result result;
  using Interval_set = boost::icl::interval_set<int>;
  using Interval = Interval_set::interval_type;
  Interval_set intervals;

  for (int i = 0; i + degree <= diff.size(); ++i) {
    auto shift = diff | std::views::drop(i) | std::views::take(degree);
    double head = shift[0];
    auto shift_pattern =
        shift | std::views::transform([head](double v) { return v / head; });

    bool is_slip = true;
    for (int j = 0; j != degree; ++j) {
      if (!(within_abs_pct(pattern[j], shift_pattern[j]))) {
        is_slip = false;
      }
    }

    if (!is_slip) {
      continue;
    } else {
      intervals.insert(Interval::closed(i, i + degree));
    }
  }

  for (auto const &itv : intervals) {
    result.intervals.emplace_back(boost::icl::lower(itv) + degree,
                                  boost::icl::upper(itv) + degree + 1);
    int idx = result.intervals.back().first - degree;
    result.cycle_slips.emplace_back(diff[idx] - diff[idx - 1]);
  }

  return result;
}

} // namespace gaa