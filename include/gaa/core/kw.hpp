#pragma once

#include <any>
#include <map>
#include <string>

#include <Eigen/Dense>

#include <gaa/core/pp.hpp>
#include <gaa/core/keywords.hpp>

namespace gaa::_kw
{

#define GAA_KW_def_tag(TYPE, NAME) NAME,
#define GAA_KW_value GAA_KW_def_tag
#define GAA_KW_ref GAA_KW_def_tag
#define GAA_KW_cref GAA_KW_def_tag
    enum class tags
    {
#include <gaa/core/kw/enums.hpp>
        COUNT
    };
#undef GAA_KW_def_tag
#undef GAA_KW_value
#undef GAA_KW_ref
#undef GAA_KW_cref

    template <tags t>
    struct traits
    {
        static constexpr char const
            *name = "",
            *type_name = "";
        using passed_arg_type = int;
        using any_storage_type = int;

        static std::any cast_any(passed_arg_type arg)
        {
            return std::any(arg);
        }

        static passed_arg_type any_cast(std::any any)
        {
            return std::any_cast<any_storage_type>(any);
        }
    };

#define GAA_KW_value(TYPE, NAME)                         \
    template <>                                          \
    struct traits<tags::NAME>                            \
    {                                                    \
        static constexpr char const                      \
            *name = #NAME,                               \
            *type_name = #TYPE;                          \
        using passed_arg_type = GAA_PP_STRIP_PARAM TYPE; \
        using any_storage_type = passed_arg_type;        \
                                                         \
        static std::any cast_any(passed_arg_type arg)    \
        {                                                \
            return arg;                                  \
        }                                                \
                                                         \
        static passed_arg_type any_cast(std::any any)    \
        {                                                \
            return std::any_cast<any_storage_type>(any); \
        }                                                \
    };
#define GAA_KW_ref(TYPE, NAME)                                                    \
    template <>                                                                   \
    struct traits<tags::NAME>                                                     \
    {                                                                             \
        static constexpr char const                                               \
            *name = #NAME,                                                        \
            *type_name = #TYPE;                                                   \
        using passed_arg_type = GAA_PP_STRIP_PARAM TYPE &;                        \
        using any_storage_type = std::reference_wrapper<GAA_PP_STRIP_PARAM TYPE>; \
                                                                                  \
        static std::any cast_any(passed_arg_type arg)                             \
        {                                                                         \
            return std::ref(arg);                                                 \
        }                                                                         \
                                                                                  \
        static passed_arg_type any_cast(std::any any)                             \
        {                                                                         \
            return std::any_cast<any_storage_type>(any).get();                    \
        }                                                                         \
    };
#define GAA_KW_cref(TYPE, NAME)                                                         \
    template <>                                                                         \
    struct traits<tags::NAME>                                                           \
    {                                                                                   \
        static constexpr char const                                                     \
            *name = #NAME,                                                              \
            *type_name = #TYPE;                                                         \
        using passed_arg_type = GAA_PP_STRIP_PARAM TYPE const &;                        \
        using any_storage_type = std::reference_wrapper<GAA_PP_STRIP_PARAM TYPE const>; \
                                                                                        \
        static std::any cast_any(passed_arg_type arg)                                   \
        {                                                                               \
            return std::cref(arg);                                                      \
        }                                                                               \
                                                                                        \
        static passed_arg_type any_cast(std::any any)                                   \
        {                                                                               \
            return std::any_cast<any_storage_type>(any).get();                          \
        }                                                                               \
    };
#include <gaa/core/kw/enums.hpp>
#undef GAA_KW_value
#undef GAA_KW_ref
#undef GAA_KW_cref

#define GAA_KW_arg(NAME)                                               \
    typename traits<tags::NAME>::passed_arg_type NAME() const          \
    {                                                                  \
        return traits<tags::NAME>::any_cast(this->_get<tags::NAME>()); \
    }                                                                  \
                                                                       \
    Args &NAME(typename traits<tags::NAME>::passed_arg_type NAME)      \
    {                                                                  \
        return this->_set<tags::NAME>(NAME);                           \
    }                                                                  \
    bool _has_##NAME() const                                           \
    {                                                                  \
        return this->_has<tags::NAME>();                               \
    }
#define GAA_KW_value(TYPE, NAME) GAA_KW_arg(NAME)
#define GAA_KW_ref(TYPE, NAME) GAA_KW_arg(NAME)
#define GAA_KW_cref(TYPE, NAME) GAA_KW_arg(NAME)

    

    class Args
    {
    public:
        using map_type = std::map<tags, std::any>;

        template <tags... required_tags>
        friend class Required_tags_validator;

    private:
        map_type m_map;

    public:
        Args() = default;
        ~Args() = default;

    private:
        template <tags tag>
        bool _has() const
        {
            return m_map.contains(tag);
        }

        template <tags tag>
        std::any _get() const
        {
            gaa_assert((this->_has<tag>()), "Required argument is not given");
            return m_map.at(tag);
        }

        template <tags tag>
        Args &_set(typename traits<tag>::passed_arg_type arg)
        {
            gaa_assert((!this->_has<tag>()), "You can't assign same argument more than once");
            m_map.insert_or_assign(tag, traits<tag>::cast_any(arg));
            return *this;
        }

    public:
#include <gaa/core/kw/enums.hpp>
    };

#undef GAA_KW_value
#undef GAA_KW_ref
#undef GAA_KW_cref

    template <tags... required_tags>
    class Required_tags_validator
    {
    public:
        using wrapped_type = Args;
    
    private:
        wrapped_type const & m_args;

        template <tags tag>
        void validate()
        {
            gaa_assert(m_args.template _has<tag>(), "Required arg is not given");
        }
    
    public:
        ~Required_tags_validator() = default;
        Required_tags_validator(wrapped_type const & args) 
        : m_args(args)
        {
            (validate<required_tags>(), ...) ;
        }

        wrapped_type const & unwrap() const 
        {
            return m_args;
        }
    };
}

namespace gaa
{
    using kw_tags = _kw::tags;
    using kw = _kw::Args;
    using kwargs = kw const &;

    template <kw_tags... required_tags>
    using kwargs_require = _kw::Required_tags_validator<required_tags...>;
}
