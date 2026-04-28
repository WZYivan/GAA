#pragma once

#include <gaa/core/config.hpp>

#if !(GAA_WITH_R)
#error                                                                         \
    "GAA is configured without R, it's impossible to embed R, please rebuild GAA"
#else

#include <concepts>
#include <cstddef>
#include <vector>

#include <RInside.h>
#include <Rcpp.h>

#define GAA_INIT_R(INTERPRETER) RInside INTERPRETER

namespace gaa {
namespace R = ::Rcpp;

using _rcpp_Vector_not_supported_type = void;

template <class V> struct _r_vector_gen {
  using type = _rcpp_Vector_not_supported_type;
};
template <> struct _r_vector_gen<double> {
  using type = R::NumericVector;
};
template <> struct _r_vector_gen<int> {
  using type = R::IntegerVector;
};
template <> struct _r_vector_gen<char> {
  using type = R::CharacterVector;
};
template <> struct _r_vector_gen<bool> {
  using type = R::LogicalVector;
};

template <class V>
  requires(!std::same_as<typename _r_vector_gen<V>::type,
                         _rcpp_Vector_not_supported_type>)
using r_vector = typename _r_vector_gen<V>::type;

template <class V, class Alloc>
r_vector<V> std2r(std::vector<V, Alloc> const &vec) {
  r_vector<V> r_vec(vec.size());
  for (std::size_t i = 0; i != vec.size(); ++i) {
    r_vec[i] = vec[i];
  }
  return r_vec;
}
} // namespace gaa

#endif
