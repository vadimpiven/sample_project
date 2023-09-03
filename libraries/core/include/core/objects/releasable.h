// SPDX-License-Identifier: Apache-2.0 OR MIT

#pragma once

#include <core/error_handling/suppressions.h>
#include <core/objects/non_copiable.h>

#include <spdlog/spdlog.h>

#include <concepts>
#include <exception>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace core {

/// Use std::unique_ptr with custom deleter instead whenever possible
/// Warning: any exception thrown by Releaser would be consumed
template <typename Type, typename Releaser = std::function<void(Type)>>
    requires std::destructible<Type>
          && std::is_nothrow_move_assignable_v<Type>
          && std::is_nothrow_move_constructible_v<Type>
          && std::invocable<Releaser, Type>
class Releasable : public NonCopiable
{
public:
    constexpr Releasable() noexcept = default;

    template <typename Invalid = Type>
        requires std::equality_comparable_with<Type, Invalid>
    [[nodiscard]] constexpr Releasable(Type && value, Invalid && invalid, Releaser && release)
        : m_value(std::nullopt)
        , m_release(std::move(release))
    {
        if (!(value == invalid))
        {
            [[likely]]
            m_value = std::move(value);
        }
    }

    [[nodiscard]] constexpr Releasable(Releasable && other) noexcept
        : m_value(std::move(other.m_value))
        , m_release(std::move(other.m_release))
    {}

    constexpr void Swap(Releasable & other) noexcept
    {
        if (this != &other)
        {
            [[likely]]
            std::swap(m_value, other.m_value);
            std::swap(m_release, other.m_release);
        }
    }

    constexpr Releasable & operator=(Releasable && other) noexcept
    {
        Swap(other);
        return *this;
    }

    template<typename OtherType, typename OtherReleaser>
        requires std::destructible<OtherType>
                 && std::is_nothrow_move_assignable_v<OtherType>
                 && std::is_nothrow_move_constructible_v<OtherType>
                 && std::invocable<OtherReleaser, OtherType>
    friend class Releasable;

    template<typename SameType = Type, typename SameReleaser = Releaser, typename OtherReleaser>
        requires std::same_as<SameReleaser, std::function<void(SameType)>>
              && std::is_constructible_v<std::function<void(SameType)>, OtherReleaser>
    constexpr Releasable & operator=(Releasable<SameType, OtherReleaser> && other) noexcept
    {
        Release();
        m_value.swap(other.m_value);
        m_release = std::function<void(Type)>(std::move(other.m_release));
        return *this;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return m_value.operator bool();
    }

    [[nodiscard]] constexpr bool HasValue() const noexcept
    {
        return m_value.has_value();
    }

    [[nodiscard]] constexpr const Type & Value() const & noexcept
    {
        return m_value.value();
    }

    [[nodiscard]] constexpr Type * operator->() noexcept
    {
        return m_value.operator->();
    }

    [[nodiscard]] constexpr const Type * operator->() const noexcept
    {
        return m_value.operator->();
    }

    [[nodiscard]] constexpr const Type & operator *() const & noexcept
    {
        return *m_value;
    }

    constexpr void Release() noexcept
    {
        if (m_value)
        {
            [[likely]]
            try
            {
                UNUSED(std::invoke(m_release, *std::move(m_value)));
            }
            catch (const std::exception & err)
            {
                SPDLOG_ERROR("Releasable release failed, error: {}", err.what());
            }
            m_value.reset();
        }
    }

    constexpr ~Releasable() noexcept
    {
        Release();
    }

private:
    std::optional<Type> m_value;
    Releaser m_release;
};

template <typename Type, typename Invalid, typename Releaser>
Releasable(Type && /*value*/, Invalid && /*invalid*/, Releaser && /*release*/) -> Releasable<Type, Releaser>;

} // namespace core
