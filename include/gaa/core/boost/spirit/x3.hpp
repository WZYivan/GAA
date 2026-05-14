#pragma once

#include <ranges>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/spirit/home/x3.hpp>

#include <gaa/core/keywords.hpp>
#include <gaa/core/utils.hpp>

namespace gaa::spirit::x3 {
namespace x3 = boost::spirit::x3;
using namespace boost::spirit::x3;

decltype(auto) parse(std::string const &str, auto rule) {
  return x3::parse(str.begin(), str.end(), rule);
}

decltype(auto) parse(std::string_view sv, auto rule) {
  return x3::parse(sv.begin(), sv.end(), rule);
}

struct _attr_str_fn {
  std::string &cnt;
  mutable bool is_trim = false;

  void operator()(auto const &ctx) const {
    if (is_trim) {
      std::string str = x3::_attr(ctx);
      boost::trim(str);
      cnt.swap(str);
    } else {
      cnt = x3::_attr(ctx);
    }
  }

  decltype(auto) trim(bool val) const {
    is_trim = val;
    return *this;
  }
};
struct _attr_str_gen {
  _attr_str_fn operator()(std::string &cnt) const {
    return _attr_str_fn{.cnt = cnt};
  }
};
inline constexpr _attr_str_gen p2str{};

struct _dbl_fn {
  double &dbl;

  void operator()(auto const &ctx) const {
    static boost::regex dbl_pattern(R"(^[+-]?\d+\.\d+$)");
    static boost::regex sci_pattern(R"(^[+-]?\d+\.\d+[DEe][+-]?\d+$)");
    std::string cnt{x3::_attr(ctx)};
    boost::trim(cnt);

    if (cnt.empty()) {
      dbl = 0.0;
      return;
    }

    bool is_dbl = boost::regex_match(cnt, dbl_pattern);
    if (!is_dbl) {
      bool is_sci = boost::regex_match(cnt, sci_pattern);
      if (is_sci) {
        boost::replace_all(cnt, "D", "E");
      }
      gaa_assert(
          is_dbl || is_sci,
          std::format(
              "Pattern Error: {:s} can't be treated as double (pattern:{:s})",
              cnt, dbl_pattern.str()));
    }

    gaa_assert(from_str(cnt, dbl),
               std::format("convert from `{:s}` to double failed", cnt));
  }
};
template <std::ranges::range R>
  requires std::same_as<std::ranges::range_value_t<R>, double>
struct _dbl_range_fn {
  R &r;

  void operator()(auto const &ctx) {
    double val;
    _dbl_fn{.dbl = val}(ctx);
    *std::back_inserter(r) = val;
  }
};
struct _dbl_gen {
  _dbl_fn operator()(double &dbl) const { return _dbl_fn{.dbl = dbl}; }

  template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, double>
  _dbl_range_fn<R> operator()(R &r) const {
    return _dbl_range_fn{.r = r};
  }
};
inline constexpr _dbl_gen p2dbl{};

struct _itg_fn {
  int &itg;

  void operator()(auto const &ctx) const {
    static boost::regex itg_pattern(R"(^[+-]?\d+$)");
    std::string cnt{x3::_attr(ctx)};
    boost::trim(cnt);

    if (cnt.empty()) {
      itg = 0;
      return;
    }

    bool is_itg = boost::regex_match(cnt, itg_pattern);
    gaa_assert(is_itg,
               std::format(
                   "Pattern Error: {:s} can't be treated as int (pattern:{:s})",
                   cnt, itg_pattern.str()));

    gaa_assert(from_str(cnt, itg),
               std::format("convert from `{:s}` to integer failed", cnt));
  }
};
template <std::ranges::range R>
  requires std::same_as<std::ranges::range_value_t<R>, int>
struct _itg_range_fn {
  R &r;

  void operator()(auto const &ctx) {
    int val;
    _itg_fn{.itg = val}(ctx);
    *std::back_inserter(r) = val;
  }
};
struct _itg_gen {
  _itg_fn operator()(int &itg) const { return _itg_fn{.itg = itg}; }

  template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, int>
  _itg_range_fn<R> operator()(R &r) const {
    return _itg_range_fn{.r = r};
  }
};
inline constexpr _itg_gen p2int{};

struct _chr_fn {
  char &chr;

  void operator()(auto const &ctx) const {
    std::string cnt{x3::_attr(ctx)};
    gaa_assert(cnt.size() == 1,
               std::format("Pattern Error: {:s} is not a char", cnt));
    chr = cnt.front();
  }
};
template <std::ranges::range R>
  requires std::same_as<std::ranges::range_value_t<R>, char>
struct _chr_range_fn {
  R &r;

  void operator()(auto const &ctx) {
    char val;
    _chr_fn{.chr = val}(ctx);
    *std::back_inserter(r) = val;
  }
};
struct _chr_gen {
  _chr_fn operator()(char &chr) const { return _chr_fn{.chr = chr}; }

  template <std::ranges::range R>
    requires std::same_as<std::ranges::range_value_t<R>, char>
  _chr_range_fn<R> operator()(R &r) const {
    return _chr_range_fn{.r = r};
  }
};
inline constexpr _chr_gen p2chr{};

template <std::size_t Size>
inline decltype(auto) vchr = x3::repeat(Size)[x3::char_];

template <std::size_t Size, std::size_t Repeat>
inline decltype(auto) vchrs = x3::repeat(Repeat)[vchr<Size>];

template <std::size_t Size>
inline decltype(auto) skip = x3::repeat(Size)[x3::omit[x3::char_]];
} // namespace gaa::spirit::x3