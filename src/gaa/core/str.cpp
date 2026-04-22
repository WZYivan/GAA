#include <cctype>
#include <gaa/core/str.hpp>
#include <gaa/core/kw.hpp>

namespace gaa
{
    std::vector<std::string> split(std::string_view sv, std::string_view delim)
    {
        return std::views::split(sv, delim) | std::ranges::to<std::vector<std::string>>();
    }

    std::string_view trim_left(std::string_view sv)
    {
        auto it = sv.begin();
        for (; it != sv.end() && std::isspace(*it); ++it)
        {
        }
        return std::string_view(it, sv.end());
    }

    std::string_view trim_right(std::string_view sv)
    {
        auto it = sv.rbegin();
        std::size_t drop = 0;
        for (; it != sv.rend() && std::isspace(*it); ++it, ++drop)
        {
        }
        return sv.substr(0, sv.length() - drop + 1);
    }

    std::string_view trim(std::string_view sv)
    {
        return trim_left(trim_right(sv));
    }
}