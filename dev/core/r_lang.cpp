#include <gaa/wrap/R/rlang.hpp>

int main(int argc, char **argv) {
#if GAA_WITH_R
  gaa::init_r_interp();

  std::vector<double> dbls{1.1, 2.3, 4.5};
  auto r_vec = gaa::std2r(dbls);
  Rcpp::print(r_vec);
  Rcpp::print(Rcpp::wrap(dbls));

  gaa::end_r_interp();
  gaa::r_interp();

  // SEGMATATION FAULT cause R is endded
  auto _ = Rcpp::NumericVector::create(1);
#endif
}