#include <string>
#include <print>

#include <gaa/core/message.hpp>

namespace gaa
{

    enum Identity
    {
        X,
        Y
    };

    struct S
    {
        std::string message = "Sender";
        Identity identity = X;
    };

    struct A
    {
        std::string prefix = "Accepter";
        Identity identity = X;
    };

    // template <>
    // struct Build_channel<S, A>
    // {
    //     static bool acknowledge(S const &s, A const &a)
    //     {
    //         return s.identity == a.identity;
    //     }

    //     static void eval(S const &s, A const &a)
    //     {
    //         std::println("{:s}(of A) => {:s}(of S)", a.prefix, s.message);
    //     }
    // };

    GAA_channel_begin((S), (A));
    GAA_channel_acknowledge(s, a)
    {
        return s.identity == a.identity;
    }
    GAA_channel_eval((void), s, a)
    {
        std::println("{:s}(of A) => {:s}(of S)", a.prefix, s.message);
        return;
    }
    GAA_channel_end();
}

int main()
{
    static_assert(std::same_as<void, gaa::Channel<gaa::S, gaa::A>::eval_type>);

    gaa::S{} >> gaa::A{};
    gaa::S{"", gaa::Y} >> gaa::A{"", gaa::X};
}