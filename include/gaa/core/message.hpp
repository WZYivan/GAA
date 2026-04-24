#pragma once

#include <type_traits>

#include <gaa/core/keywords.hpp>
#include <gaa/core/pp.hpp>

namespace gaa
{
    template <class Sender, class Accepter>
    struct Build_channel
    {
        struct _channel_has_not_built_t
        {
        };

        static bool acknowledge(Sender const &s, Accepter const &a);
        static int eval(Sender const &s, Accepter const &a);
    };

    template <class SenderP, class AccepterP>
    struct _channel_traits
    {
        using sender_type = SenderP;
        using accepter_type = AccepterP;
        using channel_base = Build_channel<sender_type, accepter_type>;
        using eval_type = std::invoke_result_t<decltype(&channel_base::eval), sender_type, accepter_type>;
    };

    template <class Sender, class Accepter>
    struct Channel
        : public Build_channel<Sender, Accepter>,
          public _channel_traits<Sender, Accepter>
    {
    };

    template <class Sender, class Accepter>
    concept Has_Channel = !requires {
        typename Build_channel<Sender, Accepter>::_channel_has_not_built_t;
    } && requires(Sender const &s, Accepter const &a) {
        { Build_channel<Sender, Accepter>::acknowledge(s, a) } -> std::convertible_to<bool>;
        Build_channel<Sender, Accepter>::eval(s, a);
    };

    template <class Ret, class Sender, class Accepter>
    concept Has_Channel_R = std::convertible_to<
        typename _channel_traits<Sender, Accepter>::eval_type,
        Ret>;

    template <class Sender, class Accepter>
    concept Has_Weak_Channel = requires(Sender const &s, Accepter const &a) {
        s >> a;
    };

    template <class Ret, class Sender, class Accepter>
    concept Has_Weak_Channel_R = requires(Sender const &s, Accepter const &a) {
        { s >> a } -> std::convertible_to<Ret>;
    };

    template <class Sender, class Accepter>
        requires Has_Channel<Sender, Accepter>
    decltype(auto) operator>>(Sender const &s, Accepter const &a)
    {
        gaa_assert((Channel<Sender, Accepter>::acknowledge(s, a)), "Channel acknowledge failed");
        return Channel<Sender, Accepter>::eval(s, a);
    }

}

#define GAA_channel_begin(SENDER_TYPE, ACCEPTER_TYPE)                                      \
    template <>                                                                            \
    struct Build_channel<GAA_PP_STRIP_PARAM SENDER_TYPE, GAA_PP_STRIP_PARAM ACCEPTER_TYPE> \
    {                                                                                      \
    private:                                                                               \
        using _sender_in_building_t = GAA_PP_STRIP_PARAM SENDER_TYPE;                      \
        using _accepter_in_building_t = GAA_PP_STRIP_PARAM ACCEPTER_TYPE

#define GAA_channel_acknowledge(SENDER_ARG, ACCEPTER_ARG) \
public:                                                   \
    static bool acknowledge(_sender_in_building_t const &SENDER_ARG, _accepter_in_building_t ACCEPTER_ARG)

#define GAA_channel_eval(RET_TYPE, SENDER_ARG, ACCEPTER_ARG) \
public:                                                      \
    static GAA_PP_STRIP_PARAM RET_TYPE eval(_sender_in_building_t const &SENDER_ARG, _accepter_in_building_t ACCEPTER_ARG)

#define GAA_channel_end() \
    }

#define GAA_channel_forward(S1, A1, S2, A2)                                                    \
    GAA_channel_begin(S1, A1);                                                                 \
    GAA_channel_acknowledge(s, a)                                                              \
    {                                                                                          \
        return Build_channel<GAA_PP_STRIP_PARAM S2, GAA_PP_STRIP_PARAM A2>::acknowledge(s, a); \
    }                                                                                          \
    GAA_channel_eval((decltype(auto)), s, a)                                                   \
    {                                                                                          \
        return Build_channel<GAA_PP_STRIP_PARAM S2, GAA_PP_STRIP_PARAM A2>::eval(s, a);        \
    }                                                                                          \
    GAA_channel_end();

#define GAA_weak_channel(RET, SENDER, SENDER_ARG, ACCEPTER, ACCEPTER_ARG) \
    GAA_PP_STRIP_PARAM RET operator>>(GAA_PP_STRIP_PARAM SENDER SENDER_ARG, GAA_PP_STRIP_PARAM ACCEPTER ACCEPTER_ARG)
