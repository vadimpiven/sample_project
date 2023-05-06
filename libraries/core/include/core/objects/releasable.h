#pragma once

#include <core/objects/non_copiable.h>

#include <concepts>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

#include <core_export.h>

namespace core {

/// Use std::unique_ptr with custom deleter instead whenever possible
template <typename T, typename Releaser = std::function<void(T)>, T Invalid = T()>
    requires std::destructible<T>
          && std::is_nothrow_copy_constructible_v<T>
          && std::is_nothrow_move_constructible_v<T>
          && std::equality_comparable<T>
          && std::invocable<Releaser, T>
class CORE_EXPORT Releasable : public NonCopiable
{
public:
    constexpr Releasable()
        : m_release()
        , m_value(Invalid)
        , m_invalid(Invalid)
    {}

    Releasable(T && value, Releaser && release, T && invalid = Invalid)
        : m_release(std::move(release))
        , m_value(std::move(value))
        , m_invalid(std::move(invalid))
    {}

    constexpr Releasable(Releasable && other) noexcept
        : m_release(std::move(other.m_release))
        , m_value(std::move(other.m_value))
        , m_invalid(std::move(other.invalid))
    {}

    constexpr Releasable & operator=(Releasable && other) noexcept
    {
        if (this == &other)
            return *this;

        std::swap(m_release, other.m_release);
        std::swap(m_value, other.m_value);
        std::swap(m_invalid, other.m_invalid);
        return *this;
    }

    [[nodiscard]] constexpr bool Initialized() const noexcept
    {
        return m_value != m_invalid;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return Initialized();
    }

    [[nodiscard]] T operator *() const
    {
        if (!Initialized())
        {
            throw std::logic_error(__func__ + std::string(" not initialized"));
        }
        return m_value;
    }

    void Release() noexcept
    {
        if (Initialized())
        {
            try
            {
                (void) std::invoke(m_release, m_value);
            }
            catch (...) {}
            m_value = m_invalid;
        }
    }

    ~Releasable() noexcept
    {
        Release();
    }

private:
    Releaser m_release;
    T m_value;
    T m_invalid;
};

} // namespace core
