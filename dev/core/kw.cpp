#include "gaa/core/keywords.hpp"
#include <gaa/core/kw.hpp>

void foo(gaa::kwargs_require<gaa::kw_tags::converge_threshold> args)
{
    gaa_assert(args.unwrap().converge_threshold() == 10);
}

int main()
{
    foo(gaa::kw{}.converge_threshold(10));
}