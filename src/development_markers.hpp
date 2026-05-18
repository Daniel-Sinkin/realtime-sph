// src/development_markers.hpp

#ifndef REALTIME_SPH_SRC_DEVELOPMENT_MARKERS_HPP
#define REALTIME_SPH_SRC_DEVELOPMENT_MARKERS_HPP

#pragma once

#include <type_traits>

#if defined(mut)
#    error "mut marker macro is already defined"
#endif

#if defined(cpy)
#    error "cpy marker macro is already defined"
#endif

#if defined(def)
#    error "def marker macro is already defined"
#endif

#if defined(__clang__)
#    define mut [[clang::annotate("mut")]]
#    define cpy [[clang::annotate("cpy")]]
#else
#    define mut
#    define cpy
#endif

#define def auto

namespace realtime_sph
{
template <typename T>
[[nodiscard]] constexpr def copy(const T& value) -> std::remove_cvref_t<T>
{
    return value;
}
}  // namespace realtime_sph

#endif  // REALTIME_SPH_SRC_DEVELOPMENT_MARKERS_HPP
