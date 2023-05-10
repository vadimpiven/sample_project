#pragma once

#include <core/objects/non_copiable.h>

#include <concepts>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

#include <core_export.h>

namespace core {

/// Use std::unique_ptr with custom deleter instead whenever possible
/// Warning: any exception thrown by Releaser would be consumed
template <typename Type, typename Releaser = std::function<void(Type)>>
    requires std::destructible<Type>
          && std::is_nothrow_move_constructible_v<Type>
          && std::invocable<Releaser, Type>
class CORE_EXPORT Releasable : public NonCopiable
{
public:
    constexpr Releasable() = default;

    template <typename Invalid = Type>
        requires std::equality_comparable_with<Type, Invalid>
    Releasable(Type && value, Invalid && invalid, Releaser && release)
        : m_value(std::nullopt)
        , m_release(std::move(release))
    {
        if (!(value == invalid))
        {
            [[likely]]
            m_value = std::move(value);
        }
    }

    constexpr Releasable(Releasable && other) noexcept
        : m_value(std::move(other.m_value))
        , m_release(std::move(other.m_release))
    {}

    constexpr Releasable & operator=(Releasable && other) noexcept
    {
        if (this == &other)
        {
            [[unlikely]]
            return *this;
        }

        std::swap(m_value, other.m_value);
        std::swap(m_release, other.m_release);
        return *this;
    }

    template<typename OtherType, typename OtherReleaser>
        requires std::destructible<OtherType>
                 && std::is_nothrow_move_constructible_v<OtherType>
                 && std::invocable<OtherReleaser, OtherType>
    friend class Releasable;

    template<typename SameType = Type, typename SameReleaser = Releaser, typename OtherReleaser>
        requires std::same_as<SameReleaser, std::function<void(SameType)>>
              && std::is_constructible_v<std::function<void(SameType)>, OtherReleaser>
    Releasable & operator=(Releasable<SameType, OtherReleaser> && other)
    {
        std::swap(m_value, other.m_value);
        m_release = std::function<void(Type)>(other.m_release);
        return *this;
    }

    [[nodiscard]] constexpr bool operator !() const noexcept
    {
        return !m_value.has_value();
    }

    [[nodiscard]] Type operator *() const
    {
        return m_value.value();
    }

    void Release() noexcept
    {
        if (m_value.has_value())
        {
            [[likely]]
            try
            {
                (void) std::invoke(m_release, m_value.value());
            }
            catch (...) {}
            m_value.reset();
        }
    }

    ~Releasable() noexcept
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
