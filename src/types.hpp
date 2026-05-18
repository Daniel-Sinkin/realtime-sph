#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>

#if defined(__has_include)
#    if __has_include(<stdfloat>)
#        include <stdfloat>
#    endif
#endif

namespace ds_template
{
using usize = std::size_t;
using isize = std::ptrdiff_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uptr = std::uintptr_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using iptr = std::intptr_t;

#if defined(__STDCPP_FLOAT32_T__) and defined(__STDCPP_FLOAT64_T__)
using f32 = std::float32_t;
using f64 = std::float64_t;
#else
using f32 = float;
using f64 = double;
#endif

static_assert(sizeof(f32) == 4zu);
static_assert(sizeof(f64) == 8zu);

inline constexpr auto k_invalid_index = std::numeric_limits<usize>::max();
inline constexpr auto k_invalid_id = std::numeric_limits<u32>::max();

template <typename T>
concept TimeInt = std::same_as<std::remove_cvref_t<T>, int>;

template <typename Lhs, typename Rhs>
concept HasPlus = requires(Lhs lhs, Rhs rhs) { lhs + rhs; };

template <typename Lhs, typename Rhs>
concept HasMultiply = requires(Lhs lhs, Rhs rhs) { lhs * rhs; };

struct TimeDelta
{
    int value{};

    [[nodiscard]] constexpr auto operator<=>(const TimeDelta&) const noexcept = default;
};

struct TimePoint
{
    int value{};

    [[nodiscard]] constexpr auto operator<=>(const TimePoint&) const noexcept = default;
};

[[nodiscard]] constexpr auto operator+(TimeDelta lhs, TimeDelta rhs) noexcept -> TimeDelta
{
    return TimeDelta{lhs.value + rhs.value};
}

[[nodiscard]] constexpr auto operator-(TimeDelta lhs, TimeDelta rhs) noexcept -> TimeDelta
{
    return TimeDelta{lhs.value - rhs.value};
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator*(TimeDelta lhs, T rhs) noexcept -> TimeDelta
{
    return TimeDelta{lhs.value * rhs};
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator*(T lhs, TimeDelta rhs) noexcept -> TimeDelta
{
    return rhs * lhs;
}

[[nodiscard]] constexpr auto operator+(TimePoint lhs, TimeDelta rhs) noexcept -> TimePoint
{
    return TimePoint{lhs.value + rhs.value};
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator+(TimePoint lhs, T rhs) noexcept -> TimePoint
{
    return lhs + TimeDelta{rhs};
}

[[nodiscard]] constexpr auto operator-(TimePoint lhs, TimeDelta rhs) noexcept -> TimePoint
{
    return TimePoint{lhs.value - rhs.value};
}

[[nodiscard]] constexpr auto operator-(TimePoint lhs, TimePoint rhs) noexcept -> TimeDelta
{
    return TimeDelta{lhs.value - rhs.value};
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator==(TimeDelta lhs, T rhs) noexcept -> bool
{
    return lhs.value == rhs;
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator<=>(TimeDelta lhs, T rhs) noexcept
{
    return lhs.value <=> rhs;
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator==(T lhs, TimeDelta rhs) noexcept -> bool
{
    return rhs == lhs;
}

template <TimeInt T>
[[nodiscard]] constexpr auto operator<=>(T lhs, TimeDelta rhs) noexcept
{
    return lhs <=> rhs.value;
}

[[nodiscard]] constexpr auto is_time_point_value(i64 value) noexcept -> bool
{
    return static_cast<i64>(std::numeric_limits<int>::min()) <= value
           and value <= static_cast<i64>(std::numeric_limits<int>::max());
}

[[nodiscard]] constexpr auto to_time_point(i64 value) noexcept -> std::optional<TimePoint>
{
    if (not is_time_point_value(value)) return std::nullopt;
    return TimePoint{static_cast<int>(value)};
}

static_assert(requires(TimePoint point, TimeDelta delta, int value) {
    { point + delta } -> std::same_as<TimePoint>;
    { point + value } -> std::same_as<TimePoint>;
    { point - delta } -> std::same_as<TimePoint>;
    { point - point } -> std::same_as<TimeDelta>;
    { delta * value } -> std::same_as<TimeDelta>;
});
static_assert(not HasPlus<TimePoint, TimePoint>);
static_assert(not HasMultiply<TimePoint, int>);
static_assert(not HasPlus<TimePoint, unsigned>);
}  // namespace ds_template
