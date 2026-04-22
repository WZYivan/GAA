#pragma once

#include <format>
#include <ranges>
#include <vector>
#include <string_view>

namespace gaa
{
    template <std::ranges::range R>
    std::string str(R &&r, std::string_view fmt)
    {
        std::string s;
        auto out = std::back_inserter(s);

        s.append("[");
        for (auto it = r.begin();;)
        {
            if constexpr (
                requires {
                    r.begin()->first;
                    r.begin()->second;
                })
            {
                auto const &p = *it;
                std::format_to(out, "{{");
                std::vformat_to(out, fmt, std::make_format_args(p.first, p.second));
                std::format_to(out, "}}");
            }
            else
            {
                std::vformat_to(out, fmt, std::make_format_args(*it));
            }

            if (++it != r.end())
            {
                std::format_to(out, ", ");
            }
            else
            {
                break;
            }
        }
        s.append("]");

        return s;
    }

    extern std::vector<std::string> split(std::string_view sv, std::string_view delim);
    extern std::string_view trim_left(std::string_view);
    extern std::string_view trim_right(std::string_view);
    extern std::string_view trim(std::string_view);
}