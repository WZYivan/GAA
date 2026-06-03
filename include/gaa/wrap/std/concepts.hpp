#pragma once

#include <map>
#include <string>
#include <vector>

namespace gaa {
struct True {
  constexpr static bool value = true;
};
struct False {
  constexpr static bool value = false;
};

template <class T> struct Is_vector : public False {};
template <class V, class Alloc>
struct Is_vector<std::vector<V, Alloc>> : public True {};
template <class V> struct Is_vector<std::initializer_list<V>> : public True {};
template <class T> inline constexpr bool is_vector_v = Is_vector<T>::value;
template <class T>
concept Is_Vector = is_vector_v<T>;

template <class T> struct Is_string : public False {};
template <class CharT, class Traits, class Alloc>
struct Is_string<std::basic_string<CharT, Traits, Alloc>> : public True {};
template <class T> inline constexpr bool is_string_v = Is_string<T>::value;
template <class T>
concept Is_String = is_string_v<T>;

template <class T> struct Is_map : public False {};
template <class K, class V, class Comp, class Alloc>
struct Is_map<std::map<K, V, Comp, Alloc>> : public True {};
template <class T> inline constexpr bool is_map_v = Is_map<T>::value;
template <class T>
concept Is_Map = is_map_v<T>;

template <class T> struct Is_idx_map : public False {};
template <class V, class Comp, class Alloc>
struct Is_idx_map<std::map<std::size_t, V, Comp, Alloc>> : public True {};
template <class T> inline constexpr bool is_idx_map_v = Is_map<T>::value;
template <class T>
concept Is_Idx_Map = is_idx_map_v<T>;

template <class T> struct Is_str_map : public False {};
template <class V, class Comp, class Alloc>
struct Is_str_map<std::map<std::string, V, Comp, Alloc>> : public True {};
template <class T> inline constexpr bool is_str_map_v = Is_map<T>::value;
template <class T>
concept Is_Str_Map = is_str_map_v<T>;

template <class T> struct Is_pair : public False {};
template <class K, class V> struct Is_pair<std::pair<K, V>> : public True {};
template <class T> inline constexpr bool is_pair_v = Is_pair<T>::value;
template <class T>
concept Is_Pair = is_pair_v<T>;

template <class T> struct Is_tuple : public False {};
template <class... Ts> struct Is_tuple<std::tuple<Ts...>> : public True {};
template <class T> inline constexpr bool is_tuple_v = Is_tuple<T>::value;
template <class T>
concept Is_Tuple = is_tuple_v<T>;

template <class T> struct Is_scalar {
  constexpr static inline bool value =
      !std::ranges::range<T> && !is_pair_v<T> && !is_tuple_v<T>;
};
template <class T> inline constexpr bool is_scalar_v = Is_scalar<T>::value;
template <class T>
concept Is_Scalar = is_scalar_v<T>;

template <class T>
concept Has_Push_Back = requires(T &t) {
  typename T::value_type;
  t.push_back(std::declval<typename T::value_type>());
};

template <class T>
concept Has_Size = requires(T &t) {
  { t.size() } -> std::same_as<std::size_t>;
};
} // namespace gaa