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
template <typename T, typename Releaser = std::function<void(T)>>
    requires std::destructible<T>
          && std::is_nothrow_move_constructible_v<T>
          && std::invocable<Releaser, T>
class CORE_EXPORT Releasable : public NonCopiable
{
public:
    constexpr Releasable() = default;

    template <typename I = T>
        requires std::equality_comparable_with<T, I>
    Releasable(T && value, I && invalid, Releaser && release)
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

    template<typename T1, typename Function>
        requires std::destructible<T1>
                 && std::is_nothrow_move_constructible_v<T1>
                 && std::invocable<Function, T1>
    friend class Releasable;

    template<typename T1 = T, typename Releaser1 = Releaser, typename Function>
        requires std::same_as<Releaser1, std::function<void(T1)>>
              && std::is_constructible_v<std::function<void(T1)>, Function>
    Releasable & operator=(Releasable<T1, Function> && other)
    {
        std::swap(m_value, other.m_value);
        m_release = std::function<void(T)>(other.m_release);
        return *this;
    }

    [[nodiscard]] constexpr bool operator !() const noexcept
    {
        return !m_value.has_value();
    }

    [[nodiscard]] T operator *() const
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
    std::optional<T> m_value;
    Releaser m_release;
};

template <typename T, typename I, typename Releaser>
Releasable(T && /*value*/, I && /*invalid*/, Releaser && /*release*/) -> Releasable<T, Releaser>;

} // namespace core
