
#ifndef GNSSTK_EXPORT_H
#define GNSSTK_EXPORT_H

#ifdef GNSSTK_STATIC_DEFINE
#  define GNSSTK_EXPORT
#  define GNSSTK_NO_EXPORT
#else
#  ifndef GNSSTK_EXPORT
#    ifdef GNSSTK_EXPORTS
        /* We are building this library */
#      define GNSSTK_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define GNSSTK_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef GNSSTK_NO_EXPORT
#    define GNSSTK_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef GNSSTK_DEPRECATED
#  define GNSSTK_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef GNSSTK_DEPRECATED_EXPORT
#  define GNSSTK_DEPRECATED_EXPORT GNSSTK_EXPORT GNSSTK_DEPRECATED
#endif

#ifndef GNSSTK_DEPRECATED_NO_EXPORT
#  define GNSSTK_DEPRECATED_NO_EXPORT GNSSTK_NO_EXPORT GNSSTK_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef GNSSTK_NO_DEPRECATED
#    define GNSSTK_NO_DEPRECATED
#  endif
#endif

#endif /* GNSSTK_EXPORT_H */
