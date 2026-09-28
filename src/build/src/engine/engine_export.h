
#ifndef BALOO_ENGINE_EXPORT_H
#define BALOO_ENGINE_EXPORT_H

#ifdef BALOO_ENGINE_STATIC_DEFINE
#define BALOO_ENGINE_EXPORT
#define BALOO_ENGINE_NO_EXPORT
#else
#ifndef BALOO_ENGINE_EXPORT
#ifdef KF6BalooEngine_EXPORTS
/* We are building this library */
#define BALOO_ENGINE_EXPORT __attribute__((visibility("default")))
#else
/* We are using this library */
#define BALOO_ENGINE_EXPORT __attribute__((visibility("default")))
#endif
#endif

#ifndef BALOO_ENGINE_NO_EXPORT
#define BALOO_ENGINE_NO_EXPORT __attribute__((visibility("hidden")))
#endif
#endif

#ifndef BALOO_ENGINE_DEPRECATED
#define BALOO_ENGINE_DEPRECATED __attribute__((__deprecated__))
#endif

#ifndef BALOO_ENGINE_DEPRECATED_EXPORT
#define BALOO_ENGINE_DEPRECATED_EXPORT BALOO_ENGINE_EXPORT BALOO_ENGINE_DEPRECATED
#endif

#ifndef BALOO_ENGINE_DEPRECATED_NO_EXPORT
#define BALOO_ENGINE_DEPRECATED_NO_EXPORT BALOO_ENGINE_NO_EXPORT BALOO_ENGINE_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef BALOO_ENGINE_NO_DEPRECATED
#define BALOO_ENGINE_NO_DEPRECATED
#endif
#endif

#endif /* BALOO_ENGINE_EXPORT_H */
