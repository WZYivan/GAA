#pragma once

#include <gaa/wrap/std/variant/variant.hpp>
#include <gaa/wrap/std/variant/visitor.hpp>

namespace gaa {
using variant::Variable;
using variant::Variant;
template <Variable V> using Type_of_variable = variant::Variable_t<V>;
template <class T>
inline constexpr Variable variable_of_type = variant::variable_of_v<T>;
} // namespace gaa