
#ifndef GAA_API_H
#define GAA_API_H

#ifdef GAA_STATIC_DEFINE
#  define GAA_API
#  define GAA_NO_EXPORT
#else
#  ifndef GAA_API
#    ifdef gaa_EXPORTS
        /* We are building this library */
#      define GAA_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define GAA_API __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef GAA_NO_EXPORT
#    define GAA_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef GAA_DEPRECATED
#  define GAA_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef GAA_DEPRECATED_EXPORT
#  define GAA_DEPRECATED_EXPORT GAA_API GAA_DEPRECATED
#endif

#ifndef GAA_DEPRECATED_NO_EXPORT
#  define GAA_DEPRECATED_NO_EXPORT GAA_NO_EXPORT GAA_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef GAA_NO_DEPRECATED
#    define GAA_NO_DEPRECATED
#  endif
#endif

#endif /* GAA_API_H */
