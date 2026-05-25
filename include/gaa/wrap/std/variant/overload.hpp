#pragma once

namespace gaa {
namespace variant {
template <class... Fs> struct _overloaded : public Fs... {
  using Fs::operator()...;

  using cur_overload = _overloaded<Fs...>;
  template <class F> using nxt_overload = _overloaded<F, cur_overload>;
  template <class F> using overload_for = nxt_overload<F>;

  template <class F> overload_for<F> overload(F &&f) const {
    return overload_for<F>{f};
  }
};
template <class... Fs> _overloaded(Fs...) -> _overloaded<Fs...>;

struct _make_overloaded {
  template <class F> _overloaded<F> operator()(F &&f) const {
    return _overloaded<F>{f};
  }
  template <class... Fs> _overloaded<Fs...> operator()(Fs &&...fs) const {
    return _overloaded<Fs...>{fs...};
  }
};
constexpr inline _make_overloaded overload = {};

template <class F> using overload_for = _overloaded<F>;
} // namespace variant
} // namespace gaa