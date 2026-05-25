#pragma once

#include <map>
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
template <class T> inline constexpr bool is_vector_v = Is_vector<T>::value;
template <class T>
concept Is_Vector = is_vector_v<T>;

template <class T> struct Is_map : public False {};
template <class K, class V, class Comp, class Alloc>
struct Is_map<std::map<K, V, Comp, Alloc>> : public True {};
template <class T> inline constexpr bool is_map_v = Is_map<T>::value;
template <class T>
concept Is_Map = is_map_v<T>;

template <class T> struct Is_pair : public False {};
template <class K, class V> struct Is_pair<std::pair<K, V>> : public True {};
template <class T> inline constexpr bool is_pair_v = Is_pair<T>::value;
template <class T>
concept Is_Pair = is_pair_v<T>;
} // namespace gaa