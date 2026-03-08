
#ifndef PNNA_EXPORT_H
#define PNNA_EXPORT_H

#ifdef PNNA_BUILT_AS_STATIC
#  define PNNA_EXPORT
#  define PNNA_NO_EXPORT
#else
#  ifndef PNNA_EXPORT
#    ifdef pnna_EXPORTS
        /* We are building this library */
#      define PNNA_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define PNNA_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef PNNA_NO_EXPORT
#    define PNNA_NO_EXPORT 
#  endif
#endif

#ifndef PNNA_DEPRECATED
#  define PNNA_DEPRECATED __declspec(deprecated)
#endif

#ifndef PNNA_DEPRECATED_EXPORT
#  define PNNA_DEPRECATED_EXPORT PNNA_EXPORT PNNA_DEPRECATED
#endif

#ifndef PNNA_DEPRECATED_NO_EXPORT
#  define PNNA_DEPRECATED_NO_EXPORT PNNA_NO_EXPORT PNNA_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PNNA_NO_DEPRECATED
#    define PNNA_NO_DEPRECATED
#  endif
#endif

#endif /* PNNA_EXPORT_H */
