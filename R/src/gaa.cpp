#include <gaa/gnss/space/sv_pos_from_broadcast.hpp>

// [[Rcpp::export]]
Rcpp::List sv_pos_from_broadcast(Rcpp::List const &list, double t) {
  return gaa::sv_pos_from_broadcast(list, t);
}