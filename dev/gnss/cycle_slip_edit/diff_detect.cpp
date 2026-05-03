#include <gaa/gnss/cycle_slip_edit.hpp>
#include <print>

int main() {
  std::vector<double> vals{
      -148779.995, -157896.944, -167055.676, -176257.943,
      -185502.014, -194783.190, -204602.723, -213961.067,
      -223355.647, -232787.456, -242254.851, -251755.044,
  };

  {
    int i = 0;
    for (auto val : vals) {
      std::println("[{}]:{} ", i++, val);
    }
  }
  gaa::Cycle_detect_result result = gaa::diff_detect(vals, 5);
  for (auto [itv, slip] :
       std::views::zip(result.intervals, result.cycle_slips)) {
    std::println("[{}, {}) : {}", itv.first, itv.second, slip);
  }
}