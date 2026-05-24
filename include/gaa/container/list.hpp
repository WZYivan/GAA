#pragma once

#include <ranges>

#include <gaa/core/keywords.hpp>
#include <gaa/wrap/c++/type.hpp>
#include <gaa/wrap/std/variant.hpp>

#include <gaa/core/signature/incomplete.hpp>

namespace gaa {
template <class Kp = String, class Vp = Variant>
class List : public Dict<Kp, Vp> {
public:
  using Base = Dict<Kp, Vp>;

private:
public:
  using Base::Base;

  using Base::at;
  using Base::contains;
  using Base::size;

  template <class T> decltype(auto) append(Kp const &key, T &&v) {
    Base::insert_or_assign(key,
                           std::forward<Variant>(Variant(std::forward<T>(v))));
  }

  template <class V> bool is(Kp const &key) const {
    gaa_assert(Base::contains(key), "key \'{}\' not exists", key);
    return std::holds_alternative<V>(Base::at(key));
  }

  template <class V, class Self>
  decltype(auto) at(this Self &&self, Kp const &key) {
    gaa_assert(self.template is<V>(key), "variant type mismatch");
    return std::get<V>(self.at(key));
  }

  decltype(auto) keys() const {
    return std::views::transform(
        *this, [](auto const &kv) -> Kp const & { return kv.first; });
  }

  template <class Self, class Visitor>
  decltype(auto) visit(this Self &&self, Kp const &key, Visitor &&visitor) {
    return std::visit(visitor, self.at(key));
  }

  String fmt() const {
    String str;
    auto out = std::back_inserter(str);
    for (auto const &k : this->keys()) {
      this->visit(
          k, gaa::make_visitor([&]<class T>(T &&v) {
            using Decay = std::decay_t<T>;
            if constexpr (std::formattable<Decay, Char> &&
                          !gaa::is_vector<Decay>) {
              std::format_to(out, "{} = {}\n", k, v);
            } else if constexpr (gaa::is_vector<Decay> &&
                                 std::formattable<typename Decay::value_type,
                                                  Char>) {
              std::format_to(out, "{} = [", k);
              for (auto const &v : v) {
                std::format_to(out, "{}, ", v);
              }
              str.pop_back();
              str.pop_back();
              std::format_to(out, "]\n");
            } else {
              std::format_to(out, "{} = <unformattable>", k);
            }
          }));
    }

    return str;
  }
};
} // namespace gaa