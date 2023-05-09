#pragma once

#include <core/objects/non_copiable.h>

#include <concepts>
#include <functional>
#include <optional>
#include <utility>

#include <core_export.h>

namespace core {

/// Use std::unique_ptr with custom deleter instead whenever possible
template <typename T, typename Releaser = std::function<void(T)>>
    requires std::destructible<T>
          && std::is_nothrow_copy_constructible_v<T>
          && std::is_nothrow_move_constructible_v<T>
          && std::equality_comparable<T>
          && std::invocable<Releaser, T>
class CORE_EXPORT Releasable : public NonCopiable
{
public:
    constexpr Releasable() = default;

    Releasable(T && value, T && invalid, Releaser && release)
        : m_value(std::move(value))
        , m_release(std::move(release))
    {
        if (m_value.value() == invalid)
        {
            [[unlikely]]
            m_value.reset();
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

} // namespace core
