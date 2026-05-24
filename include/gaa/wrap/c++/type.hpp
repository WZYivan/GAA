#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace gaa {

template <class> struct _is_vector_base {};
template <class V, class Alloc = std::allocator<V>>
struct _vector : public std::vector<V, Alloc>,
                 public _is_vector_base<_vector<V, Alloc>> {
  using Base = std::vector<V, Alloc>;
  using Base::Base;
};

template <class V>
inline constexpr bool is_vector = std::is_base_of_v<_is_vector_base<V>, V>;

namespace basic_types {
using Char = char;
using Integer = std::int64_t;
using Numeric = double;
using String = std::string;
using Index = std::size_t;
using Size = Index;

template <class V, class Alloc = std::allocator<V>>
using Vector = _vector<V, Alloc>;

using Chars = Vector<Char>;
using Integers = Vector<Integer>;
using Numerics = Vector<Numeric>;
using Strings = Vector<String>;
using Indices = Vector<Index>;

template <class K, class V> using Dict = std::map<K, V>;
} // namespace basic_types
using namespace basic_types;

template <class T, class... Args>
inline constexpr bool is_one_of_v = (std::same_as<T, Args> || ...);

template <class V>
concept Basic_Type = is_one_of_v<V, Char, Integer, Numeric, String, Chars,
                                 Index, Integers, Numerics, Strings, Indices>;
} // namespace gaa