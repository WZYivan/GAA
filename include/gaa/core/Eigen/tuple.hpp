#pragma once

#include <tuple>
#include <Eigen/Dense>

#include <gaa/core/keywords.hpp>

namespace gaa
{
    template <class Xpr_1>
    decltype(auto) make_tuple(Eigen::MatrixBase<Xpr_1> const &m_1)
        requires(
            Xpr_1::RowsAtCompileTime != Eigen::Dynamic &&
            Xpr_1::ColsAtCompileTime != Eigen::Dynamic &&
            (Xpr_1::RowsAtCompileTime == 1 || Xpr_1::ColsAtCompileTime == 1))
    {
        constexpr auto Size = Xpr_1::RowsAtCompileTime == 1 ? Xpr_1::ColsAtCompileTime : Xpr_1::RowsAtCompileTime;
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        {
            return std::make_tuple(m_1[I]...);
        }(std::make_index_sequence<static_cast<std::size_t>(Size)>());
    }

    template <class T, class... Args>
        requires(std::convertible_to<T, Args> && ...)
    decltype(auto) make_vector(std::tuple<T, Args...> tuple)
    {
        constexpr std::size_t Size = sizeof...(Args) + 1;
        using Vec = Eigen::Vector<T, Size>;
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        {
            return Vec({std::get<I>(tuple)...});
        }(std::make_index_sequence<static_cast<std::size_t>(Size)>());
    }
}