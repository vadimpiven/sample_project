#pragma once

#include <core_export.h>

namespace core {

struct CORE_EXPORT NonCopiable
{
    NonCopiable(const NonCopiable &) = delete;
    NonCopiable & operator=(const NonCopiable &) = delete;

protected:
    NonCopiable() = default;
};

} // namespace core
